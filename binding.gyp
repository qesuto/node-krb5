{
  "targets": [
    {
      "target_name": "krb5",
      "include_dirs": [
        "$(MITKRB5)/include",
        "$(MITKRB5)/include/krb5",
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/krb5.cc"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "conditions": [
              [
                "target_arch=='x64'",
                {
                  "link_settings": {
                    "libraries": [
                      "$(MITKRB5)/lib/amd64/krb5_64.lib"
                    ]
                  }
                }
              ],
              [
                "target_arch=='ia32'",
                {
                  "link_settings": {
                    "libraries": [
                      "$(MITKRB5)/lib/i386/krb5_32.lib"
                    ]
                  }
                }
              ]
            ]
          }
        ],
        [
          "OS!='win'",
          {
            "link_settings": {
              "libraries": [
                "-lkrb5"
              ]
            }
          }
        ]
      ]
    }
  ]
}

