# shader_replace_unsafe

**Quick links:** [documentation](https://yal.cc/docs/gm/shader_replace_unsafe/)
  · [itch.io](https://yellowafterlife.itch.io/gamemaker-shader-replace-simple)  
**Versions:** GameMaker 2022+  
**Platforms:** Windows, Windows (YYC)

Hey! Feel like you need a little _danger_ in your game development?
Tired of extensions that Just Work?

Well look no further, 'cause this extension could spontaneously combust at any moment.

But it's not just about catching fire, think about the advantages: this extension can load new shaders at runtime! And not just replace a shader with an equivalent one like shader_replace_simple does, but actually add a brand new shader that will work just like the existing ones - complete with uniform functions and all that.

## What's interesting here

For this extension, "interesting" is certainly a good descriptor.

Loading a shader at runtime requires a [huge pile of boilerplate code](shader_replace_unsafe/shader_add.cpp) (there are multiple steps, and each one may fail), but that's not all!

To make a shader behave like a built-in one, we have to add it to a built-in shader arrays too!

And no one's going to just hand it to us, so we have to [find it inside built-in functions](shader_replace_unsafe/init.cpp)!

And that means [parsing raw CPU instructions](shader_replace_unsafe/instr_tools.cpp) with a slightly-obscure Windows DLL!

And only _then_ we can replace the built-in shader arrays with our own that we can expand.

## Building

See [BUILD.md](BUILD.md)

## Meta

**Author:** [YellowAfterlife](https://github.com/YellowAfterlife)  
**License:** Custom license (see `LICENSE`)