idf_component_register(
        SRCS "bpm280.c" "vcnl4040.c"
        INCLUDE_DIRS "include"
        REQUIRES bmp280 shared
)
