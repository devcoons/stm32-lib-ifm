# IFM LIBRARY

STM32 optimized library for the Internal Flash Memory Structured Data Storage

# Supported Hardware
- NUCLEO-L552ZE-Q

# Functions Guide
- `ifm_area_is_valid` : checks whether a memory area is validated or not.
- `ifm_retrieve`: retrieves data from the memory.
- `ifm_invalidate_application_upgrade_info` : invalidates the memory area that stores the application upgrade info.

# How To Use
- Add `lib-ifm` to your project.
- Enable the FLASH in the `.ioc` file.
- Modify the `lib_ifm_config.h.template` as needed and rename it `lib_ifm_config.h`. One of the following must be chosen:
    - #define IFM_AS_BOOTLOADER
    - #define IFM_AS_APPLICATION   
    - #define IFM_AS_NONE
