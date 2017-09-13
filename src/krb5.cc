#include <nan.h>
#include <string>
#include <cstring>
#include <node.h>
#include <v8.h>
extern "C" {
#include <krb5.h>
}

using Nan::AsyncWorker;
using Nan::HandleScope;
using Nan::Callback;
using Nan::AsyncQueueWorker;
using Nan::GetFunction;
using Nan::Null;
using Nan::New;
using Nan::Set;
using Nan::Utf8String;
using v8::String;
using v8::FunctionTemplate;

class krb5Worker : public AsyncWorker {
    public:
        krb5Worker(Callback *callback, const std::string& a_principal, const std::string& a_password):
            AsyncWorker(callback), m_principal(a_principal), m_password(a_password) {}
    
        ~krb5Worker() {}

        void Execute () {
            krb5_context context;
            krb5_principal principal;
            krb5_creds creds;

            int err;
            err = krb5_init_context(&context);
            if(err == 0)
            {
                err = krb5_parse_name(context, m_principal.c_str(), &principal);
                if(err == 0)
                {
                    err = krb5_get_init_creds_password(
                            context, 
                            &creds,
                            principal,
                            (char*) m_password.c_str(),
                            NULL,
                            NULL,
                            0,
                            NULL,
                            NULL
                            );

                    if(err == 0)
                    {
                        krb5_free_cred_contents(context, &creds);
                    }

                    krb5_free_principal(context, principal);
                }

                if(err != 0){  
                    const char* msg = krb5_get_error_message(context, err);
                    SetErrorMessage(msg);
                    krb5_free_error_message(context, msg);
                }

                krb5_free_context(context);
            }
            else {
                SetErrorMessage(std::strerror(err));
            }
        }

        void HandleOKCallback () {
            HandleScope scope;

            v8::Local<v8::Value> argv[] = { Null() };

            callback->Call(1, argv);
        }

    private:
        std::string m_principal;
        std::string m_password;
};

NAN_METHOD(Authenticate) {
    HandleScope scope;

	if(info.Length() < 3) {
        const char s[] = "too few arguments";
		info.GetReturnValue().Set(New(s).ToLocalChecked());
	} else if(!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsFunction()) {
        const char s[] = "wrong arguments";
        info.GetReturnValue().Set(New(s).ToLocalChecked());
	} else {
        Callback *callback = new Callback(info[2].As<v8::Function>());
        std::string principal(*Utf8String(info[0]));
        std::string password(*Utf8String(info[1]));

        AsyncQueueWorker(new krb5Worker(callback, principal, password));
        info.GetReturnValue().SetUndefined();
    }
}

NAN_MODULE_INIT(InitAll) {

  Set(target, New<String>("authenticate").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(Authenticate)).ToLocalChecked());
}

NODE_MODULE(krb5, InitAll);

