/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:03:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/24 14:54:35 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

namespace Utils {
	std::vector<std::string>
	vectorInitializer(const char** list) {
		std::vector<std::string> result;
		for (int i = 0; list[i]; i++)
			result.push_back(std::string(list[i]));
		return (result);
	}

// Split for general propose, delimiter one or more spaces.
	std::vector<std::string>
	splitStringToVector(const std::string& string) {
		std::vector<std::string> split;
		std::stringstream ss(string);
		std::string token;
		for (;ss >> token;)
			split.push_back(token);
		return (split);
	}


// Split config file directive line, values only
	std::vector<std::string>
	extractValues(const std::string& input) {
		std::vector<std::string> result;
		std::stringstream ss(input);
		std::string token;

		// skipping the (directive) the first element of input
		ss >> token;
		for (;ss >> token;)
			result.push_back(token);
		return (result);
	}

	void
	printServer(std::vector<Server>& serverList) {
		for (size_t i = 0; i < serverList.size(); i++) {
			std::cout << ANSI_COLOR_CYAN << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
			std::cout << "\n-=-=-=-=-=-=-=-=-=-= "<< ANSI_COLOR_GREEN << "Server " << i + 1 << " - " <<
				ANSI_COLOR_YELLOW << serverList[i].getHost() << ANSI_COLOR_RED << ":" << ANSI_COLOR_YELLOW <<
				serverList[i].getPort() << ANSI_COLOR_GREEN << " - Socket: " << ANSI_COLOR_YELLOW <<
				serverList[i].getSocket() << ANSI_COLOR_CYAN << " =-=-=-=-=-=-=-=-=-\n";
			std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n" << ANSI_COLOR_RESET;
			printMapVec(serverList[i].getServer());
			printMapMapVec(serverList[i].getLocationMap());
		}
	}

	bool
	isStringUnsignedInt(const std::string& string) {
		if (string.empty())
			return false;
		for (size_t i = 0; i < string.size(); i++) {
			if (string[i] < '0' || string[i] > '9')
				return false;
		}
		return (true);
	}

	uint32_t
	ipParserHtonl(const std::string& ip_address) {
		char* endptr;
		short dot_count = 0;
		uint32_t bytes = 0;
		std::string token;
		std::istringstream iss(ip_address);

		for (size_t i = 0; i < ip_address.size(); i++) {
			if (ip_address[i] == '.')
				dot_count += 1;
		}
		if (dot_count != 3)
			throw std::runtime_error("ERROR - Server: Invalid IP address format: " + ip_address);
		for (short i = 0; i < 4; i++) {
			// Number of '.' has already been checked.
			std::getline(iss, token, '.');
			long octet = std::strtol(token.c_str(), &endptr, 10);

			if (*endptr != '\0')
				throw std::runtime_error("ERROR - Server: IP address conversion failed: " + ip_address);
			if (octet < 0 || octet > 255)
				throw std::runtime_error("ERROR - Server: Invalid IP address, octet out of range: " + ip_address);
			bytes |= static_cast<uint32_t>(octet) << ((3 - i) * 8);
		}
		return (bytes);
	}

	std::string
	intToString(int number) {
		std::stringstream ss;
		ss << number;
		return (ss.str());
	}

	std::string
	getValueFromEnv(char** env, const std::string& var) {
		for (int i = 0; env[i]; i++) {
			std::string key(env[i]);
			if(key.substr(0, key.find('=')) == var)
				return (key.substr(key.find('=') + 1));
		}
		return ("");
	}

	void
	signal_handler(int signum) {
		if (signum == SIGINT)
			gEndLoop = 1;
	}

	bool
	isDirectory(const std::string& str) {
		return (str[str.size() - 1] == '/');
	}

	int
	isRegularFile(const std::string& path) {
		struct stat fileInfo;

		// Path doesn't exist
		if (stat(path.c_str(), &fileInfo) != 0)
			return (-1);
		//Se o arquivo for regular, isso significa que não é um diretório, dispositivo especial ou link simbólico, e a função retorna 1.
		return (((fileInfo.st_mode & S_IFMT) == S_IFREG) ? 1 : 0);
	}

	void
	createStyleIfNotExists(void) {
		std::string path(Global::pwd + "/var/www/css/styles.css");
		std::ifstream file(path.c_str());

		if (!file) {
			// Create the file if it not exists.
			std::ofstream outfile(path.c_str(), std::ofstream::out);

			outfile << "body {\n"
					<< "    font-family: Arial, sans-serif;\n"
					<< "    background-color: #f4f4f4;\n"
					<< "    color: #333;\n"
					<< "    margin: 0;\n"
					<< "    padding: 0;\n"
					<< "    display: flex;\n"
					<< "    justify-content: center;\n"
					<< "    align-items: center;\n"
					<< "    height: 100vh;\n"
					<< "    text-align: center;\n"
					<< "}\n"
					<< ".container {\n"
					<< "    padding: 20px;\n"
					<< "    border-radius: 8px;\n"
					<< "    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);\n"
					<< "    background-color: #fff;\n"
					<< "}\n"
					<< "h1 {\n"
					<< "    font-size: 36px;\n"
					<< "    margin-bottom: 10px;\n"
					<< "    color: #e74c3c;\n"
					<< "}\n"
					<< "p {\n"
					<< "    font-size: 18px;\n"
					<< "    margin-bottom: 20px;\n"
					<< "}\n"
					<< "a {\n"
					<< "    text-decoration: none;\n"
					<< "    color: #3498db;\n"
					<< "}\n"
					<< "a:hover {\n"
					<< "    text-decoration: underline;\n"
					<< "}\n";
			outfile.close();
		}
	}

	void
	createGenericErrorPage(std::ostringstream& oss, int& status_code) {
		oss << "<!DOCTYPE html>\n"
			   "<html lang=\"en\">\n"
			   "<head>\n"
			   "    <meta charset=\"UTF-8\">\n"
			   "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			   "    <title>" << status_code << "- Generic error page</title>\n"
			   "    <link rel=\"stylesheet\" href=\"/css/styles.css" << "\">\n"
			   "</head>\n"
			   "<body>\n"
			   "<div class=\"container\">\n"
			   "    <h1>Error " << status_code << "</h1>\n"
			   "    <p>Error Page.</p>\n"
			   "    <a href=\"/\">Go back to home page</a>\n"
			   "</div>\n"
			   "</body>\n"
			   "</html>\n";
	}

	void
	createListingPage(std::stringstream& html, const std::string& path) {
		html << "<html><head><title>Directory Listing</title>"
			 << "<style>"
			 << "body { font-family: Arial, sans-serif; margin: 20px; }"
			 << "table { width: 100%; border-collapse: collapse; }"
			 << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }"
			 << "th { background-color: #f2f2f2; }"
			 << "h2 {"
			 << "  color: #333;"
			 << "  text-align: center;"
			 << "  font-size: 24px;"
			 << "  font-weight: normal;"
			 << "  text-shadow: 1px 1px 1px #aaa;"
			 << "  margin-bottom: 20px;"
			 << "  padding: 10px 0;"
			 << "  border-bottom: 2px solid #eee;"
			 << "}"
			 << "</style>"
			 << "</head><body>"
			 << "<h2>Listing of " << path << "</h2>"
			 << "<table><tr><th>Filename</th><th>Type</th><th>Creation Date</th><th>Size</th></tr>";
	}
}
