/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "frontend.h"
#include "environment.h"
#include "operator_info.h"
#include "kernel_exception.h"
#include "pp.h"
#include "test.h"
using namespace lean;

static void tst1() {
    frontend f;
    f.add_uvar("tst");
    frontend c = f.mk_child();
    lean_assert(c.get_uvar("tst") == f.get_uvar("tst"));
    lean_assert(f.get_environment().has_children());
}

static void tst2() {
    operator_info op1 = infixl(name("and"), 10);
    operator_info op2 = infixr(name("implies"), 20);
    lean_assert(op1.get_precedence() == 10);
    lean_assert(op1.get_fixity() == fixity::Infixl);
    op1.add_internal_name(name{"Bool","And"});
    operator_info op3 = infixl(name("+"), 10);
    op3.add_internal_name(name{"Int", "plus"});
    op3.add_internal_name(name{"Real", "plus"});
    std::cout << op3.get_internal_names() << "\n";
    lean_assert(length(op3.get_internal_names()) == 2);
    operator_info op4 = op3.copy();
    op4.add_internal_name(name{"Complex", "plus"});
    std::cout << op4.get_internal_names() << "\n";
    lean_assert(length(op3.get_internal_names()) == 2);
    lean_assert(length(op4.get_internal_names()) == 3);
    lean_assert(op4.get_fixity() == fixity::Infixl);
    lean_assert(op4.get_op_name() == op3.get_op_name());
    lean_assert(prefix("tst", 20).get_fixity() == fixity::Prefix);
    lean_assert(postfix("!", 20).get_fixity() == fixity::Postfix);
    lean_assert(length(mixfixl({"if", "then", "else"}, 10).get_op_name_parts()) == 3);
    lean_assert(mixfixc({"if", "then", "else", "endif"}, 10).get_fixity() == fixity::Mixfixc);
    std::cout << mixfixc({"if", "then", "else", "endif"}, 10).get_op_name_parts() << "\n";
    std::cout << op4 << "\n";
    std::cout << op2 << "\n";
    std::cout << mixfixc({"if", "then", "else", "endif"}, 10) << "\n";
}

static void tst3() {
    frontend f;
    std::cout << "====================\n";
    std::cout << f << "\n";
}

static void tst4() {
    frontend f;
    formatter fmt = mk_pp_formatter(f);
    expr Bool = Const("Bool");
    context c;
    c = extend(c, "x", Bool);
    c = extend(c, "y", Bool);
    c = extend(c, "x", Bool, Var(1));
    c = extend(c, "x", Bool, Var(2));
    c = extend(c, "z", Bool, Var(1));
    c = extend(c, "x", Bool, Var(4));
    std::cout << fmt(c) << "\n";
}

static void tst5() {
    std::cout << "=================\n";
    frontend f;
    formatter fmt = mk_pp_formatter(f);
    f.add_var("A", Type());
    f.add_var("x", Const("A"));
    object const & obj = f.find_object("x");
    lean_assert(obj);
    lean_assert(obj.get_name() == "x");
    std::cout << fmt(obj) << "\n";
    object const & obj2 = f.find_object("y");
    lean_assert(!obj2);
    try {
        f.get_object("y");
        lean_unreachable();
    } catch (unknown_name_exception & ex) {
        std::cout << ex.what() << " " << ex.get_name() << "\n";
    }
}

class alien_cell : public neutral_object_cell {
    unsigned m_data[128];
public:
    alien_cell() {}
    unsigned & get_data(unsigned i) { return m_data[i]; }
    virtual char const * keyword() const { return "alien"; }
};

static void tst6() {
    std::cout << "=================\n";
    frontend f;
    environment env;
    env.add_neutral_object(new alien_cell());
    formatter fmt = mk_pp_formatter(f);
    std::cout << fmt(env) << "\n";
}

int main() {
    tst1();
    tst2();
    tst3();
    tst4();
    tst5();
    tst6();
    return has_violations() ? 1 : 0;
}
