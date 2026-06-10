add_library(
    issen
    issenclient.cpp
    issenclient.h
    issenconfig.cpp
    issenconfig.h
    issenconfigkeys.h
)
target_compile_features(issen PUBLIC cxx_std_20)
target_compile_options(issen PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(issen PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    issen
    PRIVATE "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Network>"
    PRIVATE "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Network>"
    PRIVATE context
    PRIVATE utils
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Qml>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Qml>"
    PUBLIC dictionary
    PUBLIC Qt6::Core
    PUBLIC Qt6::Network
)
