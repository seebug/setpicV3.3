file(COPY ./library/libBrightnessEqualizer.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}
    )

file(COPY ./library/libLinearSpaceCorrect.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}
    )

file(COPY ./library/libPartitionColorCorrect.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}
    )

#==============================================
file(COPY ./library/libBrightnessEqualizer.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../modules/setpic/
    )

file(COPY ./library/libLinearSpaceCorrect.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../modules/setpic/
    )

file(COPY ./library/libPartitionColorCorrect.so
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/../modules/setpic/
    )