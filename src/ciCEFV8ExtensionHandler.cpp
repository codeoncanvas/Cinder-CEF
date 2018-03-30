#include <iostream>
#include <string>
#include "ciCEFV8ExtensionHandler.h"
#include "cefListV8Converter.hpp"

ciCEFV8ExtensionHandler::ciCEFV8ExtensionHandler(CefRefPtr<CefApp> app){
    this->app = app;
}

bool ciCEFV8ExtensionHandler::Execute(const CefString &name,
                                       CefRefPtr<CefV8Value> object,
                                       const CefV8ValueList &arguments,
                                       CefRefPtr<CefV8Value> &retval,
                                       CefString &exception){

    std::string functionName = name;
    //CI_LOG_I("Execute name: " << functionName);
    std::cout << "Execute name: " << functionName << '\n';

    const auto found = bindJSFunctionNames.find(functionName);
    if (found != bindJSFunctionNames.end()) {

        // Create the message object with a prefix + name of the function.
        // There is probably a more efficient way to combine these
        const std::string jsBindPrefix = "js-bind-";
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(CefString(jsBindPrefix + functionName));

        // Retrieve the argument list object.
        CefRefPtr<CefListValue> args = message->GetArgumentList();

        convertList(arguments, args);

        // Send message
        CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
        browser->SendProcessMessage(PID_BROWSER, message);
        return true;
    }

    CefString type;

    if (arguments[1]->IsString()) {
        type = "string";
    } else if (arguments[1]->IsDouble()) {
        type = "double";
    } else if (arguments[1]->IsInt()) {
        type = "int";
    } else if (arguments[1]->IsBool()) {
        type = "bool";
    } else {
        std::cout << "CinderCEFV8ExtensionHandler received a message of unknown type." << std::endl;
        return false;
    }

    // Create the message object.
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(type);

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Message mane
    args->SetString(0, arguments[0]->GetStringValue());

    // Message value
    if (type == "string") {
        args->SetString(1, arguments[1]->GetStringValue());
    } else if (type == "double") {
        args->SetDouble(1, arguments[1]->GetDoubleValue());
    } else if (type == "int") {
        args->SetInt(1, arguments[1]->GetIntValue());
    } else if (type == "bool") {
        args->SetBool(1, arguments[1]->GetBoolValue());
    } else {
        std::cout << "cinderxCEFV8ExtensionHandler received a message of unknown type." << std::endl;
        return false;
    }

    // Send message
    CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
    browser->SendProcessMessage(PID_BROWSER, message);

    return true;
}
