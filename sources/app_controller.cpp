#include "app_controller.hpp"

UTMSApplication::UTMSApplication(DATA input_data, const string &state_file, bool load_existing_state)
    : instruction_handler_(input_data)
{
    if (!state_file.empty())
        instruction_handler_.enable_state_persistence(state_file, load_existing_state);
}

Instruction_Handler &UTMSApplication::handler()
{
    return instruction_handler_;
}

const Instruction_Handler &UTMSApplication::handler() const
{
    return instruction_handler_;
}

ANSWER UTMSApplication::execute(const QUESTION &question)
{
    return instruction_handler_.run(question);
}
