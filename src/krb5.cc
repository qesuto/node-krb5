#include <nan.h>
#include <string>
#include <cstring>
#include <node.h>
#include <v8.h>
extern "C" {
#include <krb5.h>
}


class krb5Worker : public NanAsyncWorker {
    public:
        krb5Worker(NanCallback *callback, const std::string& a_principal, const std::string& a_password):
            NanAsyncWorker(callback), m_principal(a_principal), m_password(a_password) {}
    
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
            NanScope();

            v8::Local<v8::Value> argv[] = { NanNull() };

            callback->Call(1, argv);
        }

    private:
        std::string m_principal;
        std::string m_password;
};

NAN_METHOD(Authenticate) {
    NanScope();

	if(args.Length() < 3)
	{  	
		printf("too few arguments.\n");
		NanReturnValue(NanNew<v8::String>("too few arguments"));
	}

	if(!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsFunction())
	{	
		printf("wrong arguments.\n");
		NanReturnValue(NanNew<v8::String>("wrong arguments"));
	}

    NanCallback *callback = new NanCallback(args[2].As<v8::Function>());
    std::string principal(*NanAsciiString(args[0]));
    std::string password(*NanAsciiString(args[1]));

    NanAsyncQueueWorker(new krb5Worker(callback, principal, password));
    NanReturnUndefined();
}


void init(v8::Handle<v8::Object> exports) {
  exports->Set(NanNew<v8::String>("authenticate"),
    NanNew<v8::FunctionTemplate>(Authenticate)->GetFunction());
}

NODE_MODULE(krb5, init);

