# SMI-Server
Linux server for the SMI-Bus (Standard Motor Interface)
It allows to control the roller shutter and sun blinds.

The program is for experimental use only. Use it at own risk only.
I can not guarantee that it will work witch all possible combinations.
Use the master-branch to compile

https://smiwiki.thefischer.net

## openHAB2 "binding"
to control the rollershutter with openHAB2 I created three files to get it work.

**test.items:**
```
/* Gruppen , Räume*/
Group Rolladen      "Rolladen"      <rolladen>          (Alle)

/* Rolladen */
Rollershutter Rollade_OG_Schlafen		"Schlafen Fenster"  <rollershutter>	(Rolladen)
```
  
**test.sitemap**
```
Frame label="Rolladen" {
	Group item=Rolladen label="Alle" icon="rollershutter"
}
```

**test.rules**
```
/**
 * This is a rule to control rollershutter 'Schlafen Fenster' via smi-server
 */
rule "Rule Rollade_OG_Schlafen "
	when
		Item Rollade_OG_Schlafen  received command
	then
    if (receivedCommand == STOP) sendHttpGetRequest("http://192.168.1.222:8088/SmiControl?id=9&cmd=0")
    if (receivedCommand == DOWN) {
      sendHttpGetRequest("http://192.168.1.222:8088/SmiControl?id=9&cmd=2")
      postUpdate(Rollade_OG_Schlafen , 100)
    }
    if (receivedCommand == UP) {
      sendHttpGetRequest("http://192.168.1.222:8088/SmiControl?id=9&cmd=1")
      postUpdate(Rollade_OG_Schlafen , 0)
    }
end
```
