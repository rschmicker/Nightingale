LD_LIBRARY_PATH=../install/openssl ../install/openssl/apps/openssl s_server -accept 6000 -tls1_2 -cert ./cert.pem -key ./key.pem -state -debug -msg -security_debug_verbose -cipher 'ECDHE-RSA-NIGHTGALE-SHA384:@SECLEVEL=0'

#-cipher 'ECDHE-ECDSA-AES256-GCM-SHA384' -debug -msg -security_debug_verbose
