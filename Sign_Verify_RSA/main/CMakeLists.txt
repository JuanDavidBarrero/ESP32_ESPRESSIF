idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_TARGET} "certs/private.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/public.pub" TEXT)