
if(NOT DEFINED API_IS_SDK)
	include(ChartAPI)
	sos_sdk_include_target(ReportAPI "${API_CONFIG_LIST}")
endif()
