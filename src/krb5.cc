#include <string>
#include <node.h>
#include <v8.h>
#include <krb5.h>

using namespace v8;
using namespace node;

struct AuthStruct
{
	std::string* principal;
	std::string* password;
	Persistent<Function> callback;
	std::string* error_message;
};


void doing_work (uv_work_t *req) {
	AuthStruct* authStruct = (AuthStruct*) req->data;
	krb5_context context;
	krb5_principal principal;
	krb5_creds creds;

	int err;
	err = krb5_init_context(&context);
	if(err == 0)
	{
		err = krb5_parse_name(context, authStruct->principal->c_str(), &principal);
		if(err == 0)
		{
			err = krb5_get_init_creds_password(
				context, 
				&creds,
				principal,
				(char*) authStruct->password->c_str(),
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
			const char* msg = krb5_get_error_message(context, err ); // ha bisogno di un context
			authStruct->error_message = new std::string(msg);
			krb5_free_error_message(context, msg);
		}

		krb5_free_context(context);
	}
	else {
		const char* msg = krb5_get_error_message(context, err ); // ha bisogno di un context
		authStruct->error_message = new std::string(msg);
		krb5_free_error_message(context, msg);
	}
}

void after_doing_work (uv_work_t *req) {

  AuthStruct* authStruct = (AuthStruct*) req->data;

  Handle<Value> argv[1];
  if(authStruct->error_message)
  {
  	argv[0] = String::New(authStruct->error_message->c_str());
  }
  else
  {
  	argv[0] = Undefined();
  }

  TryCatch try_catch;
  authStruct->callback->Call(Context::GetCurrent()->Global(), 1, argv);

  // cleanup
  delete authStruct->principal;
  delete authStruct->password;
  authStruct->callback.Dispose();
  if(authStruct->error_message)
  {
  		delete authStruct->error_message;
  }
  delete authStruct;
  delete req;

  if (try_catch.HasCaught())
    FatalException(try_catch);
}

Handle<Value> Method(const Arguments& args) {
	HandleScope scope;

	if(args.Length() < 3)
	{  	
		printf("too few arguments.\n");
		return scope.Close(String::New("too few arguments"));
	}

	if(!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsFunction())
	{	
		printf("wrong arguments.\n");
		return scope.Close(String::New("wrong arguments"));
	}

	AuthStruct* authStruct = new AuthStruct;

	authStruct->principal = new std::string(*String::AsciiValue(Local<String>::Cast(args[0])));
	authStruct->password = new std::string(*String::AsciiValue(Local<String>::Cast(args[1])));
	authStruct->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	authStruct->error_message = NULL;

	uv_work_t *req = new uv_work_t;
	req->data = authStruct;
	
	uv_queue_work(uv_default_loop(), req, doing_work, (uv_after_work_cb)after_doing_work);

	return scope.Close(Undefined());
}

void init(Handle<Object> target) {
	NODE_SET_METHOD(target, "authenticate", Method);
}

NODE_MODULE(krb5, init);
