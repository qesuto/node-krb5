node-krb5
=========
### Node.js native addon for simple krb5 user authentication

### !!! WARNING !!!

### The module currently DOES NOT VERIFY THE KDC and is susceptible to spoofing. You can find more details in issue #13 (thanks, Richard :)). 

### !!! WARNING !!!


The module currently builds on linux with krb5 headers and libs (tested on centos), osx (tested on lion and mountain lion) and windows (tested on windows 8.1 with MIT Kerberos)

Installation
------------

You can install with `npm`:

``` bash
$ npm install -g node-krb5
```

On windows you will need to have MIT Kerberos (http://web.mit.edu/Kerberos/dist/) installed. Remember to select the SDK option during installation.

You will also need to set the environment variable MITKRB5 to the MIT Kerberos home. 

```
set MITKRB5=C:\Program Files\MIT\Kerberos
```

How to Use
----------

``` js
var krb5 = require('node-krb5');

krb5.authenticate('username@REALM', 'password', function(err) {
  if (err) {
    console.log("Error: " + err);
  } else {
    console.log("OK");
  }
});
```

License
-------

(The MIT License)

Copyright (c) 2012-2014 Luca Fornili &lt;qesuto@gmail.com&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
