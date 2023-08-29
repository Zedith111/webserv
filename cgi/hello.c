/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hello.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/26 15:41:23 by zah               #+#    #+#             */
/*   Updated: 2023/08/29 15:53:17 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

int	main(void)
{
	printf("HTTP/1.1 200 OK\n");
	printf("Content-type: text/html\n");
	printf("\n");
	printf("<html>\n");
	printf("<body>\n");
	printf("<h1>Hello there!</h1>\n");
	printf("</body>\n");
	printf("</html>\n");
	return (0);
}
