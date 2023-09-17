/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 13:09:21 by zah               #+#    #+#             */
/*   Updated: 2023/09/13 17:21:38 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Server.hpp"
# include "serverUtils.hpp"

# define ENV_COUNT 17

struct requestData;
struct locationInfo;

int handleGet(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handlePost(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handlePut(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handleHead(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handleDelete(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);

int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf);

#endif