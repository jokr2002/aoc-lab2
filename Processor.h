#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>


class Processor {
private:
    const int register_size = 12;
    const int register_count = 3;

    std::vector<std::vector<int>> regedit;
    std::vector<std::string> commands_stack;
    std::vector<std::string> operands;

    std::string command_type;
    std::string command_register;
    std::string file;

    int PS = 0;
    int TC = 0;
    int PC = 0;

    void mov();
    void left();
    void right();

    void parse_command(const std::string& command);
    void load_file();
public:

    explicit Processor(const std::string &file);

    std::vector<int> toBIn(int dec_number);

    void print_info();
    void process(const std::string& command);
};

#endif //PROCESSOR_H
