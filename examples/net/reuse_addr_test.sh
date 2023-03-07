./reuse_addr_test --server_address1="127.0.0.1:9999" --server_address2="127.0.0.1:9999" --reuse_addr=false --listen_first=false --reuse_port=false
./reuse_addr_test --server_address1="127.0.0.1:9999" --server_address2="127.0.0.1:9999" --reuse_addr=true --listen_first=false --reuse_port=false
./reuse_addr_test --server_address1="127.0.0.1:9999" --server_address2="127.0.0.1:9999" --reuse_addr=true --listen_first=true --reuse_port=false
./reuse_addr_test --server_address1="127.0.0.1:9999" --server_address2="127.0.0.1:9999" --reuse_addr=false --listen_first=true --reuse_port=true
