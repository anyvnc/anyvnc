include(AnyVnc)

add_anyvnc_executable(anyvnc-cli main.cpp)

if(WIN32 OR WIN64)
set_target_properties(anyvnc-cli PROPERTIES LINK_FLAGS -mconsole)
endif()
