#include <Application/Application.h>

#include "Core/String/DoDoString.h"

#include "Core/Delegates/Delegates.h"

#include <fstream>

//------test for delegates------
typedef DoDo::Delegate_Event<int32_t(int32_t, int32_t)> Mouse_Move_Event;

int32_t test(int32_t a, int32_t b)
{
    std::cout << "this is normal function: " << "a + b is " << a + b << std::endl;
    return a + b;
}

class A
{
public:
    int32_t test(int32_t a, int32_t b) const
    {
        std::cout << "this is a method: " << "a + b is " << a + b << std::endl;
        return a + b;
    }

    int32_t test2(int32_t a, int32_t b) const
    {
        std::cout << "this is a method: " << "a + b is " << a + b << std::endl;
        //std::cout << a + b << std::endl;
        return a + b;
    }
};
//------test for delegates------

int main(int argc, char* argv[])
{
    if (argc < 2 || argv[1] == std::string("1"))
    {
        DoDo::DoDoUtf8String str("🐟");

        //5 = 4 + 1
        if (str.get_count() != 5 && str.get_length() != 1)
            return -1;

        //std::ofstream f("test.txt");
        //f << str.c_str();
        //f.close();
        return 0;
    }
    else if(argc < 2 || argv[1] == std::string("2"))
    {
        DoDo::DoDoUtf8String str("过一天是一天，努力奋斗！upup！");

        if (str.get_count() != 44 && str.get_length() != 18)
            return -1;
        return 0;
    }
    else if (argc < 3 || argv[1] == std::string("3"))
    {
        //------test for delegates------
        A a;
        int32_t f = 114514;

        //------delegates array------
        Mouse_Move_Event mouse_move;
        mouse_move += Mouse_Move_Event::Handler::From_Fun<&test>();
        mouse_move += Mouse_Move_Event::Handler::From_Method<A, &A::test>(&a);
        mouse_move += Mouse_Move_Event::Handler::From_Method<A, &A::test2>(&a);
        mouse_move += Mouse_Move_Event::Handler::From_Lambda([f](int32_t a, int32_t b)->int32_t {
            std::cout << "this is a lambda: " << a + b << std::endl;
            return a + b + f;
        });
        //mouse_move -= Mouse_Move_Event::Handler::From_Fun<test>();
        //mouse_move -= Mouse_Move_Event::Handler::From_Method<A, &A::test>(&a);
        //mouse_move -= Mouse_Move_Event::Handler::From_Method<A, &A::test2>(&a);
        mouse_move(1, 2);
        //mouse_move.Invoke(3, 4);
        if (mouse_move.Has(Mouse_Move_Event::Handler::From_Fun<&test>()))
        {
            std::cout << "mouse move has normal function" << std::endl;
        }
        mouse_move -= Mouse_Move_Event::Handler::From_Fun<test>();
        if (!mouse_move.Has(Mouse_Move_Event::Handler::From_Fun<&test>()))
        {
            std::cout << "mouse move does not have normal function" << std::endl;
        }
        //------delegates array------
        std::cout << "test other function" << std::endl;
        DoDo::Delegate<int32_t(int32_t, int32_t)> emitter_delegate = DoDo::Delegate<int32_t(int32_t, int32_t)>::From_Fun<&test>();
        //emitter_delegate.From_Fun<&test>();

        mouse_move.Invoke_With_Emitter(200, 200, emitter_delegate);

        mouse_move.Reset();
        //------test for delegates------
    }

    return 0;
}
