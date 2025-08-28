# CMakeList.txt: проект CMake для CanBoot; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (
	CanBoot 
	"CanBoot.cpp" 
	"CanBoot.h"
	"xserial.cpp"
	"xserial.hpp"

)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CanBoot PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
