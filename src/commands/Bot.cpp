/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vconesa- <vconesa-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 16:50:14 by vconesa-          #+#    #+#             */
/*   Updated: 2025/03/21 20:30:59 by vconesa-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

void Channel::initializeStaticData(void){
	if (quotes.empty()) {
        quotes.push_back("\"The only limit to our realization of tomorrow is our doubts of today.\" - Franklin D. Roosevelt");
        quotes.push_back("\"In the middle of difficulty lies opportunity.\" - Albert Einstein");
        quotes.push_back("\"The best way to predict the future is to invent it.\" - Alan Kay");
        quotes.push_back("\"Do what you can, with what you have, where you are.\" - Theodore Roosevelt");
        quotes.push_back("\"The only way to do great work is to love what you do.\" - Steve Jobs");
        quotes.push_back("\"It always seems impossible until it's done.\" - Nelson Mandela");
        quotes.push_back("\"Success is not final, failure is not fatal: it is the courage to continue that counts.\" - Winston Churchill");
        quotes.push_back("\"Don't watch the clock; do what it does. Keep going.\" - Sam Levenson");
        quotes.push_back("\"Happiness depends upon ourselves.\" - Aristotle");
        quotes.push_back("\"Act as if what you do makes a difference. It does.\" - William James");
    }

    if (jokes.empty()) {
        jokes.push_back("Why don't skeletons fight each other? They don't have the guts.");
        jokes.push_back("I told my computer I needed a break, and now it won't stop sending me Kit-Kats.");
        jokes.push_back("Why don't programmers like nature? It has too many bugs.");
        jokes.push_back("Parallel lines have so much in common. It's a shame they'll never meet.");
        jokes.push_back("Why did the scarecrow win an award? Because he was outstanding in his field.");
        jokes.push_back("Why don't some couples go to the gym? Because some relationships don't work out.");
        jokes.push_back("I told my wife she should embrace her mistakes. She gave me a hug.");
        jokes.push_back("Why did the math book look sad? Because it had too many problems.");
        jokes.push_back("How does a penguin build its house? Igloos it together.");
        jokes.push_back("What do you call fake spaghetti? An impasta.");
    }
}

std::string extractCommand(const std::string &message) {
    std::string command;

    // Check if the message starts with '[' and contains ']:' (this indicates a username is present)
    size_t start_pos = message.find("[");
    size_t end_pos = message.find("]:");

    // If it has a username and a colon (like [user]: !command)
    if (start_pos != std::string::npos && end_pos != std::string::npos) {
        // Extract everything after ']:'
        command = message.substr(end_pos + 2);  // Start after the `]:`
    } else {
        // Otherwise, treat the message as the command directly
        command = message;
    }

    // Trim any leading/trailing spaces or newlines
    size_t first = command.find_first_not_of(" \t\r\n");
    if (first != std::string::npos) {
        command = command.substr(first);
    }

    size_t last = command.find_last_not_of(" \t\r\n");
    if (last != std::string::npos) {
        command = command.substr(0, last + 1);
    }

    return command;
}


void Channel::Bot(const std::string &message) {
    // Commands for the bot: !time, !joke, !people, !quote, !help
	std::string command = extractCommand(message);

    if (command == "!time") {
        time_t now = time(0);
        struct tm *ltm = localtime(&now);
        char timeBuffer[100];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", ltm);

        std::string bot_message = ":TimeBot!bot@localhost PRIVMSG " + this->getName() +
            " :Current time: " + timeBuffer + "\r\n";

        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->sendMessage(bot_message, (*it)->getFd());
        }
    }
    else if (command == "!joke") {
        static bool seeded = false;
        if (!seeded) {
            srand(time(0)); // Use the current time as seed for rand()
            seeded = true;
        }
        int random_index = rand() % jokes.size();
        std::string joke_message = ":JokeBot!bot@localhost PRIVMSG " + this->getName() + " :" + jokes[random_index] + "\r\n";

        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->sendMessage(joke_message, (*it)->getFd());
        }
    }
    else if (command == "!people") {
        std::string people_message = ":PeopleBot!bot@localhost PRIVMSG " + this->getName() + " :Connected users: ";
        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            people_message += (*it)->getNickname() + " ";
        }
        people_message += "\r\n";

        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->sendMessage(people_message, (*it)->getFd());
        }
    }
    else if (command == "!quote") {
        static bool seeded = false;
        if (!seeded) {
            srand(time(0));  // Use the current time as seed for rand()
            seeded = true;
        }
        int random_index = rand() % quotes.size();
        std::string quote_message = ":QuoteBot!bot@localhost PRIVMSG " + this->getName() + " :" + quotes[random_index] + "\r\n";

        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->sendMessage(quote_message, (*it)->getFd());
        }
    }
    else if (command == "!help") {
        std::string help_message = ":HelpBot!bot@localhost PRIVMSG " + this->getName() + " :Available commands: !time, !joke, !people, !quote, !help.\r\n";
        for (clients_iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->sendMessage(help_message, (*it)->getFd());
        }
    }
}