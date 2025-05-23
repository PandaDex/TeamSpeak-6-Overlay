# Info

This branch contains a full rewrite of the app using C++ and Qt6.

# TODO

Fields with \* are required for me to merge with master

- [ ] polish websocket code. \*
- [ ] More error handling so app won't crash so often. \*
- [ ] Code optimalizations and cleanup. \*
- [x] Installer (99% NSIS) \*
- [x] Achive same functionality as electron version. \*
- [ ] Diffrent way to store config.
- [ ] Translations, currently planed are: Polish, German, Russian, Turkish and Czech

# Why the rewrite and Qt6

I decided to rewrite it for two main reasons:

- **Performance** – Electron is great for full desktop apps, but memory usage is brutal. This overlay used around 150 MB of RAM, while the Qt6 version uses around 50 MB.
- **Maintainability** – This project wasn’t originally meant to become what it is. I started working on the overlay for fun, because when me and my friends switched from Discord to TeamSpeak 6, I missed that feature. I didn’t care if something was done well or if it was throwing a bunch of errors, I just wanted a simple app to see who joined voice chat or who was currently talking. So there’s a lot that needs fixing and rethinking. Even if I stay with Electron, I would still have to rewrite the app.

# If the app was written poorly, why did you release it?

I didn’t want to release it or talk about it for a long time. I knew it would be a problem, so I waited. But over a month passed after the TeamSpeak 6 release, and there still wasn’t any app like this. After some encouragement from friends, I made a post about it on the TeamSpeak forum.

# Why did it take so long for you to finally work on the app?

I work on my projects in my free time, just for fun. A lot of my time is taken up by work and other responsibilities. At work, I already spend a lot of time looking at code and wondering what I was thinking when i wrote that shit. So in my free time, I didn’t want to spend more hours staring at code again.
