/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexical.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rboia-pe <rboia-pe@student.42porto.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 02:19:58 by rboia-pe          #+#    #+#             */
/*   Updated: 2023/04/07 02:19:58 by rboia-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static bool	is_special_char(char c);
char	*get_token_type(int type);

static bool	is_special_char(char c)
{
	if (c == '|' || c == ';' || c == '\'' || c == '\"' || \
	c == '>' || c == '<' || c == '$' || c == '&' || \
	c == '(' || c == ')' || c == '\\')
		return (true);
	return (false);
}

void	handle_backslash(t_token *token, char *input, int *i, char *special_char[2])
{
	int	begin;
	int	j;
	int	count_char;

	token->type = BACKSLASH;
	if (input[*i + 1] == '\0')
		token->error = 1;
	else if (ft_strchr(*special_char, input[*i + 1]) != NULL)
	{
		begin = *i;
		count_char = 0;
		j = -1;
		while (input[++(j)] && input[j] > 32)
		{
			if (input[j] == '\\')
			{
				j++;
				if (ft_strchr(*special_char, input[*i]) != NULL)
					count_char++;
			}
			else
				count_char++;
		}
		token->value = ft_calloc(1, sizeof(char *) * (count_char + 1));;
		(*i)--;
		while (input[++(*i)] && input[*i] > 32)
		{
			if (input[*i] == '\\')
			{
				if (ft_strchr(*special_char, input[++(*i)]) != NULL)
					token->value = ft_strjoin(token->value, ft_substr(input, *i, 1));
				else if (*special_char[1] == '1')
					token->value = ft_strjoin(token->value, ft_substr(input, --(*i), 1));
			}
			else
				token->value = ft_strjoin(token->value, ft_substr(input, *i, 1));
		}
		token->next = init_token(token->next);
		token->next->prev = token;
	}
	else
		printf("Error: Backslash isn't followed by anything\n");
}

void	handle_only_backslash(t_token *token, char *input, int *i)
{
	int	begin;
	int	j;
	int	count_char;

	token->type = BACKSLASH;
	if (input[*i + 1] == '\0')
		token->error = 1;
	else if (ft_strchr("\\", input[*i + 1]) != NULL)
	{
		begin = *i;
		count_char = 0;
		j = -1;
		while (input[++(j)] && input[j] > 32)
		{
			if (input[j] == '\\')
			{
				j++;
				if (ft_strchr("\\", input[*i]) != NULL)
					count_char++;
			}
			else
				count_char++;
		}
		token->value = ft_calloc(1, sizeof(char *) * (count_char + 1));;
		(*i)--;
		while (input[++(*i)] && input[*i] > 32)
		{
			if (input[*i] == '\\')
				if (ft_strchr("\\", input[++(*i)]) != NULL)
					token->value = ft_strjoin(token->value, ft_substr(input, *i, 1));
			else
				token->value = ft_strjoin(token->value, ft_substr(input, *i, 1));
		}
		token->next = init_token(token->next);
		token->next->prev = token;
	}
}

bool	is_same_word(char *input, int i)
{
	if (input[i] && input[i + 1] && (input[i + 1] == '\"' || input[i + 1] == '\'') || \
	ft_isalnum(input[i + 1]) == 1)
		return (true);
	else if (input[i] && input[i + 1] && input[i + 1] == '\\' && input[i + 2] != '\0' && \
	(input[i + 2] == '\'' || input[i + 2] == '\\' || input[i + 2] == '\"'))
		return (true);
	return (false);
}

void handle_quote(int quote_type, bool *quote, int *i, char *input, t_token *token)
{
	char	quote_char;
	int		begin;
	int		j;
	char	*temp;

	quote[quote_type] = !quote[quote_type];
	if (quote[quote_type] == false)
		return ;
	token->type = quote_type;
	begin = *i + 1;
	quote_char = input[*i];
	if (input[*i + 1] && input[*i + 1] == quote_char && input[*i] == quote_char)
	{
		token->type = EMPTY;
		(*i) = *i + 2;
		return ;
	}
	while (input[++(*i)])
	{
		if (quote_char == '\'' && input[*i] == quote_char)
			break;
		else if (quote_char == '\"' && input[*i + 1] && \
		input[*i] != '\\' && input[*i + 1] == quote_char)
		{
			(*i)++;
			break;
		}
	}
	if (is_same_word(input, *i) == true)
		token->same_word = true;
	j = -1;
	token->value = ft_substr(input, begin, *i - begin);
	while (quote_char == '\"' && token->value[++j] && token->value[j] != quote_char)
	{
		if (token->value[j + 1] && token->value[j] == '\\' && token->value[j + 1] == '\\')
		{
			temp = ft_calloc(1, sizeof(char *) * (ft_strlen(token->value) + 1));
			temp = ft_substr(token->value, 0, j);
			char *temp2 = ft_calloc(1, sizeof(char *) * (ft_strlen(token->value) + 1));
			temp2 = ft_memcpy(temp2, token->value + j + 1, ft_strlen(token->value) - j);
			handle_backslash(token, token->value, &j, (char *[]){"\\\"$", "1"});
			temp = ft_strjoin(temp, token->value);
			token->value = ft_strdup(temp);
			free(temp);
			if (is_same_word(input, (*i)))
				token->same_word = true;
		}
	}
	token->next = init_token(token->next);
	token->next->prev = token;
	token = token->next;
}

t_token	*special_char(char *input, t_token *token, int *i, bool quote[2])
{
	int 	begin;

	(*i) = *i - 1;
	while (input[++(*i)])
	{
		if (input[*i] == ' ')
			continue ;
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			if (input[*i] == '\'')
				handle_quote(S_QUOTE, quote, i, input, token);
			else
				handle_quote(D_QUOTE, quote, i, input, token);
			if (token->type != EMPTY)
				token = token->next;
			break ;
		}
		else if (input[*i] == '$')
		{
			begin = *i;
			while (input[++(*i)] && input[*i] != ' ')
				;
			token->type = ENV;
			token->value = ft_substr(input, begin, *i - begin);
			token->next = init_token(token->next);
			token->next->prev = token;
			token = token->next;
			break ;
		}
		else if (input[*i] == '|')
		{
			if (input[*i + 1] == '|')
			{
				token->type = OR;
				token->value = ft_strdup("||");
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
				(*i)++;
			}
			else
			{
			token->type = PIPE;
			token->value = ft_strdup("|");
			token->next = init_token(token->next);
			token->next->prev = token;
			token = token->next;
			}
			break ;
		}
		else if (input[*i] == ';')
		{
			token->type = SEMICOLON;
			token->value = ft_strdup(";");
			token->next = init_token(token->next);
			token->next->prev = token;
			token = token->next;
			break ;
		}
		else if (input[*i] == '>')
		{
			if (input[*i + 1] == '>')
			{
				token->type = REDIR_APPEND;
				(*i)++;
				while (input[++(*i)] && input[*i] == ' ')
					;
				begin = (*i);
				while (input[++(*i)] && input[*i] != ' ')
					;
				token->next = init_token(token->next);
				token->value = ft_substr(input, begin, *i - begin);
				token->next->prev = token;
				token = token->next;
			}
			else
			{
				token->type = REDIR_OUT;
				while (input[++(*i)] && input[*i] == ' ')
					;
				begin = (*i);
				while (input[++(*i)] && input[*i] != ' ')
					;
				token->value = ft_substr(input, begin, *i - begin);
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
			}
			break ;
		}
		else if (input[*i] == '<')
		{
			if (input[*i + 1] == '<')
			{
				token->type = REDIR_HEREDOC;
				(*i)++;
				while (input[++(*i)] && input[*i] == ' ')
					;
				begin = (*i);
				while (input[++(*i)] && input[*i] != ' ')
					;
				token->value = ft_substr(input, begin, *i - begin);
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
			}
			else
			{
				token->type = REDIR_IN;
				while (input[++(*i)] && input[*i] == ' ')
					;
				begin = (*i);
				while (input[++(*i)] && input[*i] != ' ')
					;
				token->value = ft_substr(input, begin, *i - begin);
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
			}
			break ;
		}
		else if (input[*i] == '&')
		{
			if (input[*i + 1] == '&')
			{
				token->type = AND;
				token->value = ft_strdup("&&");
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
				(*i)++;
			}
			else
			{
				token->type = AMPERSAND;
				token->value = ft_strdup("&");
				token->next = init_token(token->next);
				token->next->prev = token;
				token = token->next;
			}
			break ;
		}
		else if (input[*i] == '\\')
		{
			handle_backslash(token, input, i, (char *[]){"\\\"\'$&|<>();?*", "0"});
			token = token->next;
			break ;
		}
		else if (input[*i] == '(')
		{
			token->type = PARENTHESIS;
			token->value = ft_strdup("(");
			token->next = init_token(token->next);
			token->next->prev = token;
			token = token->next;
			break ;
		}
		else if (input[*i] == ')')
		{
			token->type = PARENTHESIS;
			token->value = ft_strdup(")");
			token->next = init_token(token->next);
			token->next->prev = token;
			token = token->next;
			break ;
		}
	}
	return (token);
}

int	lexical(char *input , t_shell *shell)
{
	t_token	*token;
	int		i;
	int		j;

	j = -1;
	i = -1;
	token = shell->token;
	while (input[++i])
	{
		token->quote[S_QUOTE] = false;
		token->quote[D_QUOTE] = false;
		while (input[i] == ' ')
			i++;
		if (input[i] == '\0')
			break ;
		if (is_special_char(input[i]) == true)
			token = special_char(input, token, &i, token->quote);
		else
		{
			token->type = WORD;
			token->value = ft_calloc(1, sizeof(char *));
			while (input[i] && input[i] > 32 && is_special_char(input[i]) == false)
				token->value = ft_strjoin(token->value, ft_substr(input, i++, 1));
			i--;
			token->next = (t_token *)malloc(sizeof(t_token));
			token->next->prev = token;
			token = token->next;
		}
	}
	print_token(shell->token);
	printf("_______________________\n");
	return (0);
}

void	print_token(t_token *token)
{
	t_token	*temp;

	temp = token;
	while (temp->next != NULL)
	{
		char *quote;
		if (temp->quote[S_QUOTE] == true)
			quote = "SINGLE";
		else if (temp->quote[D_QUOTE] == true)
			quote = "DOUBLE";
		else
			quote = "NONE";
		printf("%s_______________________\n", RED);
		printf("|%s Value: |%s%s%s|\n", BLUE, YELLOW, temp->value, BLUE);
		printf("%s|%s Type: %s%s\n", RED, BLUE, YELLOW, get_token_type(temp->type));
		printf("%s|%s Quote: %s%s\n", RED, BLUE, YELLOW, quote);
		printf("%s|%s Same word: %s%s\n", RED, BLUE, YELLOW, (temp->same_word == true) ? "TRUE" : "FALSE");
		printf("%s|______________________\n", RED);
		printf("%s\n", RESET);
		temp = temp->next;
	}
}

char	*get_token_type(int type)
{
	if (type == S_QUOTE)
		return ("S_QUOTE");
	else if (type == D_QUOTE)
		return ("D_QUOTE");
	else if (type == BACKSLASH)
		return ("BACKSLASH");
	else if (type == AMPERSAND)
		return ("AMPERSAND");
	else if (type == OR)
		return ("OR");
	else if (type == AND)
		return ("AND");
	else if (type == PIPE)
		return ("PIPE");
	else if (type == SEMICOLON)
		return ("SEMICOLON");
	else if (type == REDIR_OUT)
		return ("REDIR_OUT");
	else if (type == REDIR_IN)
		return ("REDIR_IN");
	else if (type == REDIR_APPEND)
		return ("REDIR_APPEND");
	else if (type == REDIR_HEREDOC)
		return ("REDIR_HEREDOC");
	else if (type == ENV)
		return ("ENV");
	else if (type == PARENTHESIS)
		return ("PARENTHESIS");
	else if (type == WORD)
		return ("WORD");
	return ("ERROR");
}
