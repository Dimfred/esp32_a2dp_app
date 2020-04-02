set( APP
   "app/app.cpp"
)

set( FREERTOSPP
   "freertospp/freertospp.cpp"
)

set( HARDWARE
   "hardware/bluetooth.cpp"
   "hardware/a2dp.cpp"
   "hardware/i2s.cpp"
   "hardware/nvs_flash.cpp"
)

set( FC
   # "fc/config.cpp"
   # "fc/time.cpp"
)

set( SOURCES
   "main.cpp"
   ${APP}
   ${FREERTOSPP}
   ${HARDWARE}
   ${FC}
)

set( INCLUDES
   "app/include"
   "freertospp/include"
   "hardware/include"
   "fc/include"
)

idf_component_register(
   SRCS ${SOURCES}
   INCLUDE_DIRS ${INCLUDES}
)

component_compile_options(
   -std=c++17
   -fpermissive
)
