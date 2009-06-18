project(hmac)

set(hmac_SRCS
	sha2.c
	hmac_sha2.c
)

add_library(hmac STATIC ${hmac_SRCS})
