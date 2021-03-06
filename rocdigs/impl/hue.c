/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "rocdigs/impl/hue_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Color.h"

#include <math.h>

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOHUEData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OHUE ----- */

/*
{"name": "Philips hue","mac": "00:17:88:12:15:cd","dhcp": true,"ipaddress": "192.168.100.175","netmask": "255.255.255.0",
"gateway": "192.168.100.99","proxyaddress": "none","proxyport": 0,"UTC": "2014-12-05T09:24:21","localtime": "2014-12-05T10:24:21",
"timezone": "Europe/Berlin","whitelist":{"000000007b9c25875ddd6a1a5ddd6a1a":{"last use date": "2014-12-04T16:07:30",
"create date": "2014-12-04T16:03:18","name": "Hue#BullittGroupLimited"},"rocrail4ever":{"last use date": "2014-12-04T16:11:56",
"create date": "2014-12-04T16:11:56","name": "rocrail"},"rocrail4light":{"last use date": "2014-12-05T09:24:21","create date":
"2014-12-04T16:13:46","name": "rocrail"}},"swversion": "01010854","apiversion": "1.2.1","swupdate":{"updatestate":0,"url":"",
"text":"","notify": false},"linkbutton": false,"portalservices": true,"portalconnection": "connected","portalstate":{"signedon": true,
"incoming": true,"outgoing": true,"communication": "disconnected"}}
 */

static iONode __parseJSON(const char* json) {
  iONode node = NULL;
  iOStrTok tok = StrTokOp.inst( json, ',' );
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* keyvalue = StrTokOp.nextToken(tok);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s", keyvalue );
  };
  StrTokOp.base.del(tok);

  return node;
}


/*
 * Example
 * method : PUT /api/<user>/lights/1/state
 * request: {"bri":42}
 */
#define RSPSIZE 4096
static char* __httpRequest( iOHUE inst, const char* method, const char* request ) {
  iOHUEData data = Data(inst);
  char* reply = NULL;
  Boolean OK = True;

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "trying to connected to %s:80", wDigInt.gethost(data->ini) );
  iOSocket sh = SocketOp.inst( wDigInt.gethost(data->ini), 80, False, False, False );
  if( SocketOp.connect( sh ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Connected to %s", wDigInt.gethost(data->ini) );

    char* httpReq = StrOp.fmt("%s HTTP/1.1\nHost: %s\nContent-Type: application/json\nContent-Length: %d\n\n%s", method, wDigInt.gethost(data->ini), StrOp.len(request), request );
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "length=%d\n%s", StrOp.len(httpReq), httpReq );
    SocketOp.write( sh, httpReq, StrOp.len(httpReq) );
    StrOp.free(httpReq);

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Read response..." );
    char str[RSPSIZE] = {'\0'};
    int idx = 0;
    SocketOp.setRcvTimeout( sh, 1000 );
    /* Read first HTTP header line: */
    OK = False;

    if( SocketOp.readln( sh, str ) ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, str );
      if( StrOp.find( str, "200 OK" ) ) {
        OK = True;
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "OK" );
      }

      /* Reading rest of HTTP header: */
      int contlen = 0;
      while( SocketOp.readln( sh, str ) && !SocketOp.isBroken( sh ) ) {
        if( str[0] == '\r' || str[0] == '\n' ) {
          break;
        }
        if( StrOp.find( str, "Content-Length:" ) ) {
          char* p = StrOp.find( str, ": " ) + 2;
          contlen = atoi( p );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "contlen = %d", contlen );
        }

        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, str );
      };

      if( OK && contlen > 0 ) {
        reply = (char*)allocMem(contlen+1);
        SocketOp.read( sh, reply, contlen );
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reply = %.200s", reply );
      }
      else if( OK ) {
        while( SocketOp.read( sh, &str[idx], 1 ) && !SocketOp.isBroken( sh ) && idx < RSPSIZE) {
          idx++;
          str[idx] = '\0';
        }
        reply = StrOp.dup(str);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "reply: %s", reply  );
      }
    }

    SocketOp.disConnect(sh);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s", wDigInt.gethost(data->ini) );
  }
  SocketOp.base.del(sh);

  return reply;

}


void __RGBtoXY(int R, int G, int B, float* x, float* y) {
  float red   = ( (float)R / 255.0 );        //R from 0 to 255
  float green = ( (float)G / 255.0 );        //G from 0 to 255
  float blue  = ( (float)B / 255.0 );        //B from 0 to 255

  if ( red > 0.04045 )
    red = pow( ( red + 0.055 ) / 1.055, 2.4);
  else
    red = red / 12.92;

  if ( green > 0.04045 )
    green = pow( ( green + 0.055 ) / 1.055, 2.4);
  else
    green = green / 12.92;

  if ( blue > 0.04045 )
    blue = pow( ( blue + 0.055 ) / 1.055, 2.4);
  else
    blue = blue / 12.92;

  float X = (float) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
  float Y = (float) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
  float Z = (float) (red * 0.000000 + green * 0.053077 + blue * 1.035763);

  *x = X / ( X + Y + Z );
  *y = Y / ( X + Y + Z );
}



static iONode __translate( iOHUE inst, iONode node ) {
  iOHUEData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {

  /* System command. */
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "system STOP" );
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "system GO" );
      iHueCmd cmd = allocMem(sizeof(struct HueCmd));
      cmd->methode = StrOp.fmt("GET /api/%s/lights", wDigInt.getuserid(data->ini));
      cmd->request = StrOp.dup("");
      ThreadOp.post( data->transactor, (obj)cmd );
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int val  = wOutput.getvalue( node );
    int hue  = wOutput.getparam( node );
    iONode color = wOutput.getcolor(node);
    Boolean blink = wOutput.isblink( node );
    Boolean colortype = wOutput.iscolortype( node );
    Boolean active = False;
    float x = 0;
    float y = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    Boolean useXY = False;

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.on ) || StrOp.equals( wOutput.getcmd( node ), wOutput.value ) )
      active = True;

    if( color != NULL ) {
      r = wColor.getred(color);
      g = wColor.getgreen(color);
      b = wColor.getblue(color);
      __RGBtoXY(wColor.getred(color), wColor.getgreen(color), wColor.getblue(color), &x, &y );
      useXY = True;
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output addr=%d active=%d cmd=%s bri=%d RGB=%d,%d,%d xy=%f,%f hue=%d",
        addr, active, wOutput.getcmd( node ), val, r, g, b, x, y, hue );

    iHueCmd cmd = allocMem(sizeof(struct HueCmd));
    cmd->methode = StrOp.fmt("PUT /api/%s/lights/%d/state", wDigInt.getuserid(data->ini), addr);
    if( active && useXY && colortype) {
      cmd->request = StrOp.fmt("{\"on\":%s, \"bri\":%d, \"alert\":\"%s\", \"xy\":[%f,%f]}", active?"true":"false", val, blink?"lselect":"none", x, y);
    }
    else if( active && hue > 0 && colortype)
      cmd->request = StrOp.fmt("{\"on\":%s, \"bri\":%d, \"alert\":\"%s\", \"hue\":%d}", active?"true":"false", val, blink?"lselect":"none", hue);
    else if( active )
      cmd->request = StrOp.fmt("{\"on\":%s, \"bri\":%d, \"alert\":\"%s\"}", active?"true":"false", val, blink?"lselect":"none");
    else
      cmd->request = StrOp.fmt("{\"on\":%s}", active?"true":"false");
    ThreadOp.post( data->transactor, (obj)cmd );

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if(  wProgram.getcmd( node ) == wProgram.pton ) {
      iHueCmd cmd = allocMem(sizeof(struct HueCmd));
      cmd->methode = StrOp.fmt("POST /api");
      cmd->request = StrOp.fmt("{\"devicetype\": \"rocrail\", \"username\": \"%s\"}", wDigInt.getuserid(data->ini));
      ThreadOp.post( data->transactor, (obj)cmd );
    }
  }
  return NULL;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOHUEData data = Data(inst);

  if( cmd != NULL ) {
    __translate( (iOHUE)inst, cmd );

    /* Cleanup */
    NodeOp.base.del(cmd);
  }

  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff, Boolean shutdown ) {
  iOHUEData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(250);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOHUEData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 1;
static int _version( obj inst ) {
  return vmajor*10000 + vminor*100 + patch;
}


static void __transactor( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOHUE     hue  = (iOHUE)ThreadOp.getParm(th);
  iOHUEData data = Data(hue);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor is started.");

  do {
    iHueCmd cmd = (iHueCmd)ThreadOp.getPost( th );
    if (cmd != NULL) {
      char* reply = __httpRequest(hue, cmd->methode, cmd->request);
      if( reply != NULL && StrOp.len(reply) > 0 ) {
        iONode node = __parseJSON(reply);
        if( StrOp.find(reply, "error") )
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error: %s", reply );
        else
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "ok: %s", reply );
      }
      StrOp.free(reply);
      StrOp.free(cmd->methode);
      StrOp.free(cmd->request);
      freeMem(cmd);
    }
    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor has stopped.");
}


/**  */
static struct OHUE* _inst( const iONode ini ,const iOTrace trc ) {
  iOHUE __HUE = allocMem( sizeof( struct OHUE ) );
  iOHUEData data = allocMem( sizeof( struct OHUEData ) );
  MemOp.basecpy( __HUE, &HUEOp, 0, sizeof( struct OHUE ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Philips HUE %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  iid   : [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bridge: [%s]", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  user  : [%s]", wDigInt.getuserid(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;
  data->transactor = ThreadOp.inst( data->iid, &__transactor, __HUE );
  ThreadOp.start( data->transactor );

  iHueCmd cmd = allocMem(sizeof(struct HueCmd));
  cmd->methode = StrOp.fmt("GET /api/%s/config", wDigInt.getuserid(data->ini));
  cmd->request = StrOp.dup("");
  ThreadOp.post( data->transactor, (obj)cmd );

  instCnt++;
  return __HUE;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/hue.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
