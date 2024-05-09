
#include <cstdio>
#include <csetjmp>
#include <vector>
#include <fstream>
#include <cassert>

#include  "ScriptLang.h"


void get_func(script_state* state)
{
    StringName name = state->pop_name();
    int out = 0;
    if (name == NAME("player"))
        out = 1;
    state->push_int(out);
}

void is_trigger_active(script_state* state)
{
    auto name = state->pop_name();
    auto ptr = (int*)state->get_user_ptr();
    if (name == StringName(NAME("leftfoot")))
        state->push_int(1);
    else
        state->push_int(0);
}



void make_vec(script_state* state)
{
    // nothing
}

void vec_dot(script_state* state)
{
    float x1 = state->pop_float();
    float y1 = state->pop_float();

    float x2 = state->pop_float();
    float y2 = state->pop_float();

    float dot = x1 * x2 + y1 * y2;
    state->push_float(dot);
}


#include <iostream>
int main()
{
    std::string code =
        "( let player_int ( get_func 'player' ) )\n"
        "( == player_int 1 )\n";

    Program prog;
    Library common;
    Library globals;
    Library anim_funcs;

    common.push_struct_def("vec2_t", "f,f");
    common.push_struct_def("entity_t", "i");
    common.push_struct_def("rayhit_t", "b,vec3_t,entity_t");
    common.push_constant_def("STANDING", script_types::int_t, script_value_t(0));
    common.push_function_def("vec_dot", "f", "vec2_t,vec2_t", vec_dot);
    common.push_function_def("make_vec", "vec2_t", "f,f", make_vec);

    globals.push_global_def("flGlobal", script_types::float_t);
    globals.push_global_def("bCrouching", script_types::bool_t);
    globals.push_global_def("flAim", script_types::float_t);
    globals.push_global_def("eAiState", script_types::int_t);

    anim_funcs.push_function_def("get_func", "int", "name", get_func);
    anim_funcs.push_function_def("is_trigger_active", "b", "entity_t,n", is_trigger_active);

    prog.push_library(&common);
    prog.push_library(&anim_funcs);
    prog.push_library(&globals);

    program_script_vars_instance input;
    input.resize(prog.num_vars());

    uint8_t flAim = prog.find_variable_index("flAim");
    uint8_t eAiState = prog.find_variable_index("eAiState");
    uint8_t bCrouching = prog.find_variable_index("bCrouching");

    input[flAim].f = 0.0;
    input[eAiState].ui32 = 1;
    input[bCrouching].ui32 = 1;

    BytecodeExpression expr;
    try {
        auto result = expr.compile(&prog, code, "entity_t");
        expr.print_instructions();
    }
    catch (CompileError er) {
        er.print();
        expr.print_instructions();
        return -1;
    }

    script_value_t value[64];
    script_state ss(value, 0, 64, nullptr);
    expr.execute(&ss, &prog, &input);
    int output = ss.pop_int();

    return output;
}
