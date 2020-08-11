#include "JSON.h"
#include <iostream>

int main() {
    // JSONString j("lol");
    JSONObject j;

    { j.add_item("lol", std::unique_ptr<JSON>(new JSONString("kek"))); }

    {
        JSONList *list = new JSONList;
        list->add_item(std::unique_ptr<JSON>(new JSONString("kek1")));
        list->add_item(std::unique_ptr<JSON>(new JSONString("kek2")));
        list->add_item(std::unique_ptr<JSON>(new JSONString("kek3")));
        j.add_item("lol2", std::unique_ptr<JSON>(list));
    }

    {
        JSONList *list2 = new JSONList;
        list2->add_item(std::unique_ptr<JSON>(new JSONString("kek4")));

        {
            JSONObject *o = new JSONObject;
            o->add_item("lol_o",
                        std::unique_ptr<JSON>(new JSONString("kek_o")));

            list2->add_item(std::unique_ptr<JSON>(o));
        }

        {
            JSONList *lst = new JSONList;
            lst->add_item(std::unique_ptr<JSON>(new JSONString("kek_l")));

            list2->add_item(std::unique_ptr<JSON>(lst));
        }

        list2->add_item(std::unique_ptr<JSON>(new JSONString("kek5")));

        list2->add_item(std::unique_ptr<JSON>(new JSONString("")));

        list2->add_item(std::unique_ptr<JSON>(new JSONObject));

        list2->add_item(std::unique_ptr<JSON>(new JSONList));

        j.add_item("lol3", std::unique_ptr<JSON>(list2));
    }

    auto str = j.pretty_print();
    std::cout << str << '\n';

    std::cout << j.to_string() << '\n';

    // str = j.to_string();

    std::cout << j["lol3"]["1"]["lol_o"].to_string() << '\n';

    JSONParser p(str);
    std::cout << p.parse()->to_string() << '\n';
    return 0;
}
