#include "Processor.h"

#include <cstring>
#include <math.h>
#include <fstream>
#include <iostream>
#include <algorithm>

Processor::Processor(const std::string& file): file(file)
{
    regedit.resize(register_count, std::vector<int>(register_size, 0));

    load_file();

    for (auto command: commands_stack)
    {
        PC++;
        process(command);
    }
}

void Processor::process(const std::string &command)
{
    parse_command(command);
    TC++;
    print_info();
    std::cin.getline(nullptr, 0);

    if (command_type == "mov")
    {
        mov();
        TC++;
        print_info();
        std::cin.getline(nullptr, 0);
    }
    else if(command_type == "left")
    {
        left();
        TC++;
        print_info();
        std::cin.getline(nullptr, 0);
    }
    else if(command_type == "right")
    {
        right();
        TC++;
        print_info();
        std::cin.getline(nullptr, 0);
    }
    else throw std::logic_error("Invalid command");

}


void Processor::parse_command(const std::string& command)
{
    operands.clear();
    command_type = command.substr(0, command.find(' '));
    command_register = command;
    std::string command_operands = command.substr(command.find(' ') + 1);

    const char* operand;
    char * cstr = new char [command_operands.length()+1];
    strcpy(cstr, command_operands.data());

    for (operand = strtok(cstr, ","); operand; operand = strtok(nullptr, ","))
        operands.emplace_back(operand);

}

void Processor::mov()
{
    if(operands.size() != 2)
        throw std::logic_error("Invalid number of operands");

    if(operands[0][0] != 'R')
        throw std::logic_error("First operand must be register address");

    int numb;
    int rgstr_numb = stoi(operands[0].substr(1));
    int i = operands[1].find_first_not_of(" \t\n");
    if(operands[1][i] == 'R')
    {
        numb = stoi(operands[1].substr(i + 1));
        if(numb < 0 || numb > register_count - 1) throw std::logic_error("Invalid register address");

        regedit[rgstr_numb] = regedit[numb];
        PS = regedit[rgstr_numb][0] == 1 ? -1 : 1;
    }
    else
    {
        numb = stoi(operands[1]);
        regedit[rgstr_numb] = toBIn(numb);
        PS = numb < 0 ? -1 : 1;
    }
}

void Processor::left()
{
    if(operands.size() < 2)
        throw std::logic_error("Invalid number of operands");

    for(int i = 1; i < operands.size(); ++i)
    {
        int k = operands[i].find_first_not_of(" \t\n");
        int rgstr_numb = stoi(operands[i].substr(k + 1));

        if(operands[i][k] != 'R')
            throw std::logic_error(std::to_string(i) + " operand must be register address");

        if(rgstr_numb < 0 || rgstr_numb > register_count - 1)
            throw std::logic_error("Invalid register address");
    }

    int offset = stoi(operands[0]);
    if(offset < 0) throw std::logic_error("offset value must be non-negative");
    for(int i = 1; i < operands.size(); ++i)
    {
        int k = operands[i].find_first_not_of(" \t\n");
        int rgstr_numb = stoi(operands[i].substr(k + 1));

        for(int j = 0; j < register_size; ++j)
        {
            if(j < register_size - offset)
                regedit[rgstr_numb][j] = regedit[rgstr_numb][j + offset];
            else
                regedit[rgstr_numb][j] = 0;
        }
    }
}

void Processor::right()
{
    if(operands.size() < 2)
        throw std::logic_error("Invalid number of operands");

    for(int i = 1; i < operands.size(); ++i)
    {
        int k = operands[i].find_first_not_of(" \t\n");
        int rgstr_numb = stoi(operands[i].substr(k + 1));

        if(operands[i][k] != 'R')
            throw std::logic_error(std::to_string(i) + " operand must be register address");

        if(rgstr_numb < 0 || rgstr_numb > register_count - 1)
            throw std::logic_error("Invalid register address");
    }

    int offset = stoi(operands[0]);
    if(offset < 0) throw std::logic_error("offset value must be non-negative");
    for(int i = 1; i < operands.size(); ++i)
    {
        int k = operands[i].find_first_not_of(" \t\n");
        int rgstr_numb = stoi(operands[i].substr(k + 1));
        for(int j = register_size; j >= 0; --j)
        {
            if(j > offset)
                regedit[rgstr_numb][j] = regedit[rgstr_numb][j - offset];
            else
                regedit[rgstr_numb][j] = 0;
        }
    }
}

std::vector<int> Processor::toBIn(int numb)
{
    std::vector<int> result;
    if(numb > pow(2, register_size - 1)) throw std::logic_error("Oh no, overflow!");
    int sign = numb / abs(numb);
    numb = abs(numb);

    while (numb != 0)
    {
        result.push_back(numb % 2);
        numb /=  2;
    }

    while(result.size() < register_size)
    {
        result.push_back(0);
    }

    if(sign == -1)
    {
        for(auto& it: result)
            it = (it + 1) % 2;

        for(int i = 0; i < register_size; ++i)
        {
            if(result[i] == 0)
            {
                result[i]++;
                break;
            }
            result[i] = 0;
        }
    }

    reverse(result.begin(), result.end());
    return result;
}


void Processor::load_file()
{
    std::ifstream f(file);
    char buff[128];
    while(!f.eof())
    {
        f.getline(buff, 128);
        if(strcmp(buff, "") != 0) commands_stack.emplace_back(buff);
    }
}

void Processor::print_info()
{
    std::cout << "IR: " << command_register << '\n';

    for(int i = 0; i < register_count; ++i)
    {
        std::cout << "R" << std::to_string(i) << ":";
        for (int j = 0; j < regedit[i].size(); j++)
        {
            if (j % 4 == 0) std::cout << ' ';
            std::cout << regedit[i][j];
        }
        std::cout << "\n";
    }

    std::cout << "PS: " << PS << '\n';
    std::cout << "PC: " << PC << '\n';
    std::cout << "TC: " << TC << '\n';
    std::cout << "_____________________________________________\n";
}



