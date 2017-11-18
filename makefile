ifndef MODULE_STM32F1_LOW_LEVEL_OPTIMIZATION
	MODULE_STM32F1_LOW_LEVEL_OPTIMIZATION = -g3 -O0
endif

MODULE_STM32F1_LOW_LEVEL_C_FILE				:= $(shell find module_stm32f1_low_level/ -maxdepth 3 -type f -name "*.c" )
MODULE_STM32F1_LOW_LEVEL_CPP_FILE			:= $(shell find module_stm32f1_low_level/ -maxdepth 3 -type f -name "*.cpp" )
MODULE_STM32F1_LOW_LEVEL_DIR				:= $(shell find module_stm32f1_low_level/ -maxdepth 3 -type d -name "*" )

MODULE_STM32F1_LOW_LEVEL_DIR				+= $(shell find module_mc_hardware_interfaces/ -maxdepth 10 -type d -name "*" )
MODULE_STM32F1_LOW_LEVEL_DIR				+= $(shell find module_freertos_for_stm32f1/ -maxdepth 10 -type d -name "*" )

MODULE_STM32F1_LOW_LEVEL_PATH				:= $(addprefix -I, $(MODULE_STM32F1_LOW_LEVEL_DIR))

MODULE_STM32F1_LOW_LEVEL_OBJ_FILE			:= $(addprefix build/obj/, $(MODULE_STM32F1_LOW_LEVEL_C_FILE))
MODULE_STM32F1_LOW_LEVEL_OBJ_FILE			+= $(addprefix build/obj/, $(MODULE_STM32F1_LOW_LEVEL_CPP_FILE))

MODULE_STM32F1_LOW_LEVEL_OBJ_FILE			:= $(patsubst %.c, %.o, $(MODULE_STM32F1_LOW_LEVEL_OBJ_FILE))
MODULE_STM32F1_LOW_LEVEL_OBJ_FILE			:= $(patsubst %.cpp, %.o, $(MODULE_STM32F1_LOW_LEVEL_OBJ_FILE))

MODULE_STM32F1_LOW_LEVEL_OBJ_FILE			+= module_stm32f1_low_level/CMSIS/device/gcc/startup_stm32f103xb.s

build/obj/module_stm32f1_low_level/%.o:	module_stm32f1_low_level/%.c
	@echo [CC] $<
	@mkdir -p $(dir $@)
	@$(CC) $(C_FLAGS) $(DEFINE_PROJ) $(USER_CFG_PATH) $(MODULE_STM32F1_LOW_LEVEL_PATH) $(MODULE_STM32F1_LOW_LEVEL_OPTIMIZATION) -c $< -o $@

build/obj/module_stm32f1_low_level/%.o:	module_stm32f1_low_level/%.cpp
	@echo [CPP] $<
	@mkdir -p $(dir $@)
	@$(CPP) $(CPP_FLAGS) $(DEFINE_PROJ) $(USER_CFG_PATH) $(MODULE_STM32F1_LOW_LEVEL_PATH) $(MODULE_STM32F1_LOW_LEVEL_OPTIMIZATION) -c $< -o $@


build/obj/module_stm32f1_low_level/%.o:	module_stm32f1_low_level/%.s
	@echo [AS] $<
	@mkdir -p $(dir $@)
	@$(AS) $(MODULE_STM32F1_LOW_LEVEL_PATH) $(DEFINE_PROJ) $(USER_CFG_PATH) $(MODULE_STM32F1_LOW_LEVEL_OPTIMIZATION) -c $< -o $@


# Добавляем к общим переменным проекта.
PROJECT_PATH			+= $(MODULE_STM32F1_LOW_LEVEL_PATH)
PROJECT_OBJ_FILE		+= $(MODULE_STM32F1_LOW_LEVEL_OBJ_FILE)