# Modern Data Hook Writeup

### Data hooking prior to 24H2
![Screenshot_1](https://raw.githubusercontent.com/dognews/ModernDatahook/refs/heads/main/resources/22H2_targetfunction.png) 
Previous data pointer swapping techniques for older windows versions consisted of getting the pointer used to indirectly call the function and swapping it for 
your hooked function. This pointer was either retrieved by pattern (qword_FFFFF97FFF06FBD8) or by offset 0x6FBD8 from win32k.sys. After retrieving the pointer 
the address it points to was changed to your custom function acting as a hook.

### Data hooking 24H2 Assembly View
![Screenshot_2](https://raw.githubusercontent.com/dognews/ModernDatahook/refs/heads/main/resources/24H2_asm_targetfunction.png) 
Win32k functions now look like this. The assembly is quite convulted so its easier to look at a reconstructed version in C.

### W32GetSessionState
![Screenshot_4](https://raw.githubusercontent.com/dognews/ModernDatahook/refs/heads/main/resources/24H2_W32GetSessionState.png) 
W32GetSessionState() is getting the session id for the calling threads process, then uses this to get the session state.
Only win32 threads have a valid sessionid and session state, meaning we have to call from a gui thread context (typical of all win32k functions)
W32SessionState is an opaque structure, I dont know the type definition and we don't need to know it to redirect functions by changing what these pointers point to.

### Data hooking 24H2 C View
![Screenshot_3](https://raw.githubusercontent.com/dognews/ModernDatahook/refs/heads/main/resources/24H2_targetfunction.png) 
The target function makes a call to W32GetSessionState then adds 136 to it and dereferences. After that it adds the offset 336LL to the new address and dereferences again, lastly it adds the offset 768LL then dereferences again to get the memory address. Almost all functions in win32k use this same convention to indirectly call functions. A pattern is easily visible when looking at many of these functions, 136 is always added to the session state then dereferenced, along with that 336LL and 768LL are not consistent, and reference different things. (W32GetSessionState + 136) appears to be a pointer to a group of function tables, with the offset (336LL) jumping to a specific function table in this group. 768LL is likely the offset of the actual function contained within the function table.

### Implementation
W32GetSessionState is exported by win32k, we can get this export by walking the PsLoadedModuleList for win32k, then calling RtlFindExportedRoutineByName on the module to get our
target function from the modules export table. Assuming we are operating in a valid win32 gui thread context, calling this function should return a proper session state. This session
allows us to do exactly what a majority of win32k functions do, find the location of the function pointer inside the sessin state's structure.

After finding this function pointer, we can read the memory to get the original functions memory address, then write to this pointers memory to change what it points to (make it point to our hook). Our hook can
call the original function after its execution completes.
