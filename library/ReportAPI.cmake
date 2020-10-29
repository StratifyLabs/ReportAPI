
if(NOT DEFINED IS_SDK)
	include(ChartAPI)
	sos_sdk_include_target(ReportAPI "${STRATIFYAPI_CONFIG_LIST}")
endif()
