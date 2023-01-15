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

## History
<details><summary><b>(click to expand)</b></summary>

<details>
<summary>Once upon a time, I thought that it could be cool to let people load shaders in Nuclear Throne Together.</summary>

> Originally purely an online multiplayer mod, over time
[NTT](https://yellowafterlife.itch.io/nuclear-throne-together)
has grown to have its own mod loader, accumulating
[hundreds of mods](https://itch.io/game-mods/tag-ntt)
and leading to me being hired to work on Nuclear Throne itself.

</details>

<details>
<summary>And <i>it was</i> cool.</summary>

> Although shaders had to be written in HLSL9 (and, later, HLSL11),
combined with vertex buffers it meant that you could do a whole lot more drawing tricks than before.

</details>

<details>
<summary>But there was a little caveat: I used <a href="https://forum.gamemaker.io/index.php?threads/graphics-utilities-extending-gamemakers-graphics-library.3296/">an extension</a> for it that ceased to work when the game was updated to GameMaker Studio 2.</summary>

> GMS1 used DirectX 9 while GMS2 used DirectX 11
(meaning that DLLs targeting D3D9 wouldn't work),
though there had also been major changes to graphics
pipeline that I didn't know of at the time.

</details>

<details>
<summary>And it was also a busy time for me.</summary>

> I had just moved to Odesa, and, surely enough,
was working on _at least_ 3 big projects.

</details>

<details>
<summary>So, after making a few prototypes with varying degree of success, I <a href="https://forum.gamemaker.io/index.php?threads/closed-paid-looking-for-a-programmer-to-port-gms1-dx9-native-extension-to-gms2-dx11.69458/">tried to hire someone</a> to do this.</summary>

> Surely enough, if something wasn't easy to do for me,
it wasn't going to be easy to find anyone to help with this.
> 
> Sometime around the end of the year I finally managed to finish the work myself after getting some advice on D3D11 specifics

</details>

<details>
<summary>Unknown to me, someone took my seeming lack of progress as an invitation to have <a href="https://forum.gamemaker.io/index.php?threads/live-shader-editor.70962/">a shot at the task</a>.</summary>

> They later admitted that they thought that I'd abandon the idea after seeing someone else accomplish this.

</details>

<details>
<summary>... and later begged me to not bundle my extension for free with GMLive, not to destroy their odds of selling theirs.</summary>

> (although this was a little silly - GMLive is one of the more popular GameMaker extensions, but I doubt that even 0.1% of all GameMaker users own a copy of it)

</details>

<details>
<summary>So I agreed to release my simpler extension as "pay $10 or ask for a key" and not release the bigger extension until they release their extension.</summary>

> The simpler extension is what became [shader_replace_simple](https://yellowafterlife.itch.io/gamemaker-shader-replace-simple).
Roughly 15% of its users requested a free copy, which is less than I was expecting.
>
> The bigger extension was the precursor of this one - the ideas were much alike, but it was much jankier. It was only used in Nuclear Throne and another work project that never saw the light of the day.

</details>

<details>
<summary>And then they never released their extension.</summary>

> The motivation is unclear, though they did mention that they felt
like simplifying game development too much was harmful to the industry as a whole, so maybe that's what it is.
>
> I finally removed the link to their work-in-progress thread from GMLive documentation over two years later.

</details>

<details>
<summary>Then GameMaker Studio 2.3 came along, and everything changed.</summary>

> Ability to get pointers to structs and methods meant that you could
finally acquire pointers to built-in functions without going through
ridiculous hacks.
>
> And GM2022's (still very poorly documented) [YYRunnerInterface](https://github.com/YoYoGames/GMEXT-Steamworks/blob/main/source/Steamworks_vs/Steamworks/Extension_Interface.h) allows functions to work with GML values directly - without the usual interop costs.

</details>

<details>
<summary>And my extension stopped working in GM2022 due to transition to x64 Windows runtime, so I figured that it's just about time to rewrite it using new tools at hand.</summary>

> My original implementation used a modification of [this snippet](https://stackoverflow.com/a/23843450/5578773), which was x86-only and was missing some of the extended instructions.
>
> In the remake I took the opportunity to rewrite the code to use DbgEng, which appears to be primarily aimed at low-level and driver developers, but also this is your best bet at decoding instructions without bundling a huge library (like Intel XED) _or_ staring at Intel's instruction reference for no less than a few days.
>
> And, of course, function pointers and my advancements in tooling (GmxGen and GmlCppExtFuncs) meant that there's far less boilerplate to write.

</details>

And now we're here.

</details>

## Building

See [BUILD.md](BUILD.md)

## Meta

**Author:** [YellowAfterlife](https://github.com/YellowAfterlife)  
**License:** Custom license (see `LICENSE`)