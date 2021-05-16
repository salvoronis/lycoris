#pragma once
#include <napi.h>

class ItemWrapWrapper : public Napi::ObjectWrap<ItemWrapWrapper> {
    public:
        ItemWrapWrapper(const Napi::CallbackInfo&);
        ItemWrapWrapper(Napi::Object);
        Napi::Value Greet(const Napi::CallbackInfo&);
        static Napi::Function GetClass(Napi::Env);
        std::string getName();

    private:
        std::string _greeterName;
};