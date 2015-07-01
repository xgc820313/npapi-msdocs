# Summary #

Mozilla/Opera/Chrome plugin for opening Microsoft Office Documents online vs. downloading and opening locally. Useful for working with Sharepoint and other WebDAV sites hosting Office documents.

## Why would you need this? ##
If you, like me, often use Sharepoint and Confluence sites and still like Mozilla Firefox to be a default browser, you probably find it inconvenient, that when you click Word or Excel documents, they are being downloaded and opened locally, rather than start straight from WebDAV folder. So did I. I tried using IETab for all Sharepoint sites, but hey, why do I need Firefox then?

Then I tried to set up Flashgot to start Word or Excel with the document URL. I found that I not only needed to recode URL from URL-ENCODE and then from UTF-8, but also Excel did no accept URLs longer than 256 characters.

And, finally, new great browser has been released - Google Chrome. It supports NPAPI plugins, just like Firefox and Opera.

So the idea came to develop an NPAPI plugin which would find system association for a particular URL (not just Word or Excel, but pretty much anything, that has a moniker registered in Windows Registry) and start what is appropriate.

In this version the plugin only understands .doc, .xls, .docx and .xlsx files, but there is nothing to prevent it from handling other files, it is just a matter of changing .rc file and recompiling!

So if you like the idea, just download it, copy to your `Program Files\Mozilla Firefox\plugins` folder and enjoy!

Or if you have anything to add, check out SVN, modify and commit it back.

Cheers,

Egor Shokurov, Project owner

P.S. Ah and by the way, you can subscribe to the users group with discussions and all the stuff..

<table cellspacing='0'>
<blockquote><tr><td>
<img src='http://groups.google.com/groups/img/3nb/groups_bar_ru.gif' alt='Группы Google' height='26' width='132' />
</td></tr>
<tr><td>
<b>NPAPI MS Office plugin Users group</b>
</td></tr>
<tr><td> <a href='http://groups.google.com/group/npapi-ms-office-plugin'>Перейти в эту группу</a> </td></tr>
</table>