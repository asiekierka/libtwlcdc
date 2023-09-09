# libtwlcdc

One day, on the [NESdev](https://www.nesdev.org/) chatroom, someone asked a question:

> So I can't use 64MB of RAM and also pressure on 3DS?

What a good question!

We [already know](https://github.com/blocksds/libnds/commit/7a83aaae114e81029250933d78d7f4c5d074b6ed) that the DSi touch controller supports Z1/Z2-based touch area measurement (often referred to as "pressure senstivity", as it effectively allows a rough estimate of pressure to be calculated). We also know that the 3DS has a DSi mode, complete with support for said touch contrller - touch area tests written for the DSi work on 3DS consoles.

Why not, then, write a library which uses existing DSi touch screen control code on a 3DS? And thus, libtwlcdc was created.

## Compatibility

This library will work on Luma3DS. It probably won't work on anything else.

## License

libtwlcdc is licensed under the zlib license.

## Credits

Thanks to [Sono](https://github.com/SonoSooS/) for having already done this ages ago, pointing out the two things I had missing for this to actually work.
