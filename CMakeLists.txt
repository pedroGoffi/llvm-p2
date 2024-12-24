# CMakeList.txt: projeto de CMake para Pietra-Lang, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.8)

# Habilite a Recarga Dinâmica para compiladores MSVC, se houver suporte.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (PietraLang)

# Adicione a origem ao executável deste projeto.
add_executable (PietraLang "PietraLang.cpp" "PietraLang.h")
set(LLVM_DIR "D:/llvm/build/cmake/modules/CMakeFiles")
set(LLVM_INCLUDE "D:/llvm/llvm/include/llvm")

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_PREFIX_PATH "${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE})
include_directories("E:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/include/llvm")
set(LLVM_COMPONENTS    
    Core
    Support
    ExecutionEngine
    Target
    TransformUtils
    Analysis

    # Adicione outros componentes conforme necessário
)
set_property(TARGET PietraLang PROPERTY CXX_STANDARD 20)
set_property(TARGET PietraLang PROPERTY CXX_STANDARD_REQUIRED ON)
add_compile_options(-g -O0)  # -g for debug info, -O0 for no optimizations

link_directories(${LLVM_LIBRARY_DIRS})
llvm_map_components_to_libnames(llvm_libs ${LLVM_COMPONENTS})
target_link_libraries(PietraLang ${llvm_libs})





# TODO: Adicione testes e instale destinos, se necessário.

