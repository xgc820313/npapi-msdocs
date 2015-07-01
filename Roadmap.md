## Make plugin screen look better ##
Replace nasty text message with dialog resource like this:

|Logo|
|Microsoft Office document is being opened.|
|Content-Type:|application/msword|
|File name:|test.docx|
|[online](Open.md)|[and open](Download.md)|
|[x](x.md) Remember the choice and do not ask me anymore|

## Develop an XPI Installer ##
Firefox installer to run on single click

## Ability to view documents in-place ##
Preview screen in browser window

## Register for all Office content-types ##
Make sure all documents are handled properly.

## Develop dynamic configuration dialog ##
Configuration dialog to set file associations and actions for each of them.

## Use browser download and authentication ##
As of right now WinInet is used to download the file. It makes sense to use Firefox or Chrome to authenticate only once. This needs research though.