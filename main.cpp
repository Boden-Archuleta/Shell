//Boden Archuleta
//A02098464

#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>

std::chrono::time_point<std::chrono::steady_clock> start, end;
std::chrono::duration<double> total;

//Parses user input
std::vector<char*> parse(std::string line)
{
        std::vector<std::string> words;
        auto c = line.begin();

        //This while loop was inspired by the code of the great Professor Sundberg
        while(c != line.end())
        {
                auto next = std::find(c, line.end(), ' ');
                if(next == line.end())
                {
                        words.emplace_back(c, next);
                        c = next;
                }
                else
                {
                        words.emplace_back(c, next);
                        c = next;
                        ++c;
                }
        }

        std::vector<char*> args;

        for(unsigned int i = 0; i < words.size(); i++)
        {
                args.emplace_back(const_cast<char*>(words[i].c_str()));
        }

        args.emplace_back(nullptr);
        return args;
}

//Sends the parsed input to the system for execution
void exe(std::vector<char*> args)
{
        auto pid = fork();

        if (pid < 0)
        {
                std::cout << "Error. Killing program." << std::endl;
                exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
                start = std::chrono::steady_clock::now();
                execvp(args[0], args.data());
                std::cout << "Argument is invalid." << std::endl;
                exit(0);
        }

        else
        {
                int dummy;
                waitpid(pid, &dummy, 0);
                end = std::chrono::steady_clock::now();
                total += (end - start);
        }
}

//displays input history
void listHistory(std::vector<std::string>* list)
{
        for(unsigned int i = 0; i < list->size(); i++)
        {
                std::cout << i << ": " << list->at(i) << std::endl;
        }
        return;
}

//executes a specific instance of the input history
void executeHistory(auto index, std::vector<std::string>*  list)
{
        unsigned int id = atoi(index);
        if(id >= list->size())
        {
                std::cout << "You have not entered that many commands yet." << std::endl;
                return;
        }
        auto args = parse(list->at(id));
        exe(args);
}

int main()
{
        std::string line;
        std::vector<std::string> list;
        total = total.zero();
        std::cout << "Custom shell V. 1.00" << std::endl;
        std::cout << "Type 'help' for a list of custom commands" << std::endl;

        while(1)
        {
                std::cout << "[cmd]:";

                //Parses input from user and puts it into args
                std::getline(std::cin, line);
                auto args = parse(line);

                //Special Cases
                if(strcmp(args[0], "exit") == 0) exit(EXIT_SUCCESS);
                else if(strcmp(args[0], "history") == 0) listHistory(&list);
                else if(strcmp(args[0], "ptime") == 0)
                {
                        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(total);
                        std::cout << total.count() << " seconds" << std::endl;
                }
                else if(strcmp(args[0], "^") == 0) executeHistory(args[1], &list);
                else if(strcmp(args[0], "help") == 0)
                {
                        std::cout << "'exit' - Closes the shell" << std::endl;
                        std::cout << "'history' - Displays previously entered commands" << std::endl;
                        std::cout << "'ptime' - Displays total time spend on child processes" << std::endl;
                        std::cout << "'^ n' - Executes the nth command made in this session" << std::endl;
                }

                //Sends args to exe function where it is executed
                else
                {
                        list.emplace_back(line);
                        exe(args);
                }
        }

        return EXIT_SUCCESS;
}

