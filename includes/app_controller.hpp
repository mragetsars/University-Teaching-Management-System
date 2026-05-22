#pragma once

#include "handler_instruction.hpp"
#include "struct_data.hpp"

class UTMSApplication
{
private:
    Instruction_Handler instruction_handler_;

public:
    UTMSApplication(DATA input_data, const string &state_file = EMPTYSTRING, bool load_existing_state = true);

    Instruction_Handler &handler();
    const Instruction_Handler &handler() const;
    ANSWER execute(const QUESTION &question);
};
