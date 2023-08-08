project("example.basic")

add_executable(${PROJECT_NAME} 
    "main.cpp"
)

add_dependencies(${PROJECT_NAME} "anthragon-core")
target_link_libraries(${PROJECT_NAME}
    "anthragon-core"
)
