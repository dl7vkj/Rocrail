/*
 Rocrail - Model Railroad Control System

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net
                    2014 Bert Havinga
 


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

/** ------------------------------------------------------------
  * Module: RocDigs
  * Object: MGBOX using MCS2 wrapper
  * ------------------------------------------------------------
  */

/*
 * CAN over TCP/IP 13 byte format:
 *
 *  |  0   |  1    | 2 | 3 |  4  | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
 *  | PRIO CMD RSP | HASH  | DLC |             DATA                 |
 *
 *
 *
 *
 */

#include <stdlib.h>
#include <time.h>
#include "rocdigs/impl/mcs2_impl.h"
#include "rocdigs/impl/mcs2/mcs2-const.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/MCS2.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Product.h"

#include "rocutils/public/addr.h"
#include "common/version.h"


static int instCnt = 0;
static int rrHash = 0x230F;
static char idname[17] = { '\0' }; // mfx loc readable name


static void __reportState(iOMCS2Data data);


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMCS2Data data = Data(inst);
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


/** ----- OMCS2 ----- */
static byte* __makeMsg( int prio, int cmd, Boolean rsp, int len, byte* buffer ) {
  int i = 0;
  byte* msg = allocMem(32);
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = (rrHash/256)&0xFF;
  msg[3]  = (rrHash%256)&0xFF;
  msg[4]  = len;
  for(i = 0; i < len; i++ )
    msg[5+i]  = buffer[i];
  return msg;
}


static Boolean __getFunctionState(iONode node, int fnchanged) {
  switch( fnchanged ) {
  case 0: return wFunCmd.isf0(node);
  case 1: return wFunCmd.isf1(node);
  case 2: return wFunCmd.isf2(node);
  case 3: return wFunCmd.isf3(node);
  case 4: return wFunCmd.isf4(node);
  case 5: return wFunCmd.isf5(node);
  case 6: return wFunCmd.isf6(node);
  case 7: return wFunCmd.isf7(node);
  case 8: return wFunCmd.isf8(node);
  case 9: return wFunCmd.isf9(node);
  case 10: return wFunCmd.isf10(node);
  case 11: return wFunCmd.isf11(node);
  case 12: return wFunCmd.isf12(node);
  case 13: return wFunCmd.isf13(node);
  case 14: return wFunCmd.isf14(node);
  case 15: return wFunCmd.isf15(node);
  case 16: return wFunCmd.isf16(node);
  case 17: return wFunCmd.isf17(node);
  case 18: return wFunCmd.isf18(node);
  case 19: return wFunCmd.isf19(node);
  case 20: return wFunCmd.isf20(node);
  case 21: return wFunCmd.isf21(node);
  case 22: return wFunCmd.isf22(node);
  case 23: return wFunCmd.isf23(node);
  case 24: return wFunCmd.isf24(node);
  case 25: return wFunCmd.isf25(node);
  case 26: return wFunCmd.isf26(node);
  case 27: return wFunCmd.isf27(node);
  case 28: return wFunCmd.isf28(node);
  }
  return False;
}


static void __setSysMsg( byte* msg, int prio, int cmd, Boolean rsp, int len, long addr, int subcmd, int subcmd2, int subcmd3, int subcmd4 ) {
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = (rrHash / 256) & 0xFF;
  msg[3]  = (rrHash % 256) & 0xFF;
  msg[4]  = len;
  msg[5]  = (addr & 0xFF000000) >> 24;
  msg[6]  = (addr & 0x00FF0000) >> 16;
  msg[7]  = (addr & 0x0000FF00) >> 8;
  msg[8]  = (addr & 0x000000FF);
  msg[9]  = subcmd;
  msg[10] = subcmd2;
  msg[11] = subcmd3;
  msg[12] = subcmd4;
}


static void __SoD( iOMCS2 inst ) {
  iOMCS2Data data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Start of Day..." );

  if( data->fbmod > 0 && data->fbmod > data->sod_fbmod) {

    if( wDigInt.getprotver( data->ini ) >= 3 ) {
      long devID = 0;
      int i = 0;
      for( i = 0; i < data->fbmod * 16; i++ ) {
        byte* out = allocMem(16);
        __setSysMsg(out, 0, 0x11, False, 4, devID + 1 + i, 0, 0, 0, 0);
        ThreadOp.post( data->writer, (obj)out );
        ThreadOp.sleep(20);
      }
    }
    else {
      long devID = 0;
      byte* out = allocMem(16);
      __setSysMsg(out, 0, 0x10, False, 5, devID, data->fbmod, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Query %d S88 sensor modules...", data->fbmod );
      ThreadOp.post( data->writer, (obj)out );
    }

    data->sod_fbmod = data->fbmod;
  }

  ThreadOp.sleep(100);

  if( wDigInt.getprotver( data->ini ) >= 2 && (data->sensorend - data->sensorbegin) > 0 ) {
    if(data->sensorbegin < data->sod_sensorbegin || data->sensorend > data->sod_sensorend) {
      byte*  msg   = allocMem(32);
      msg[0] = (CMD_ACC_SENSOR >> 7);
      msg[1]  = ((CMD_ACC_SENSOR & 0x7F) << 1 );
      msg[2]  = (rrHash / 256) & 0xFF;
      msg[3]  = (rrHash % 256) & 0xFF;
      msg[4]  = 7;
      msg[5]  = wMCS2.getfbdevid(data->mcs2ini) / 256; /* Geraetekenner */
      msg[6]  = wMCS2.getfbdevid(data->mcs2ini) % 256;
      msg[7]  = (data->sensorbegin & 0xFF00) >> 8; /* Kontaktkennung Start */
      msg[8]  = (data->sensorbegin & 0x00FF);
      msg[9]  = (data->sensorend & 0xFF00) >> 8; /* Kontaktkennung Ende */
      msg[10] = (data->sensorend & 0x00FF);
      msg[11] = 0xFF; /* Broadcast ein */
      msg[12] = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Query sensors from %d to %d for version >= 2...", data->sensorbegin, data->sensorend );
      ThreadOp.post( data->writer, (obj)msg );

      data->sod_sensorbegin = data->sensorbegin;
      data->sod_sensorend   = data->sensorend;
    }
  }
}


static iONode __translate( iOMCS2 inst, iONode node ) {
  iOMCS2Data data = Data(inst);
  iONode rsp   = NULL;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) && data->power ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System STOP" );
      data->power = False;
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      __reportState(data);
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System HALT" );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_HALT, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go )  && !data->power ) {
      byte* out = allocMem(32);
      __setSysMsg(out, 0, CAN_CMD_PING, False, 0, 0, 0, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System GO" );
      data->power = True;
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_GO, 0, 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      __reportState(data);
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      data->sod_fbmod = 0;
      data->sod_sensorbegin = 0;
      data->sod_sensorend = 0;
      __SoD(inst);

      return rsp;
    }
  }

  /* Switch command */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* out = allocMem(32);
    int module = wSwitch.getaddr1( node );
    if ( module == 0 )
      /* pada used, port will be actual cs2 lineair address */
      module = 1;

    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int swtime = wSwitch.getdelay( node ) / 10;
    int action = 1;

    if( swtime == 0 ) {
      swtime = data->swtime / 10;
    }

    Boolean dccswitch = StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_N );
    if( port == 0 )
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port - 1 + (dccswitch?0x3800:0x3000);
    /* cs 2 uses lineair addressing, address range 0x3000-0x33ff is for accessory decoders MM, 0x3800 for DCC, CS2 first address is 0, Rocrail's first is 1 */

    if ( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to turnout", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      if( wSwitch.issinglegate( node ) )
        action = 1;
      else
        gate = 0;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to straight", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      if( wSwitch.issinglegate( node ) )
        action = 0;
      else
        gate = 1;
    }
    __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, action, swtime/256, swtime%256);

    ThreadOp.post( data->writer, (obj)out );

    {
      int addr = wSwitch.getaddr1( node );
      int port = wSwitch.getport1( node );
      int gate = wSwitch.getgate1( node );
      iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

      if( port == 0 && addr > 0 )
        AddrOp.fromFADA( addr, &addr, &port, &gate );
      else if( addr == 0 && port > 0 )
        AddrOp.fromPADA( port, &addr, &port );

      wSwitch.setbus( nodeC, wSwitch.getbus( node ) );
      wSwitch.setaddr1( nodeC, wSwitch.getaddr1( node ) );
      wSwitch.setport1( nodeC, wSwitch.getport1( node ) );
      wSwitch.setgate1( nodeC, wSwitch.getgate1( node ) );
      wSwitch.setgatevalue( nodeC, action );

      if( wSwitch.getiid(node) != NULL )
        wSwitch.setiid( nodeC, wSwitch.getiid(node) );

      wSwitch.setstate( nodeC, wSwitch.getcmd( node ) );

      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }

    return rsp;
  }

  /* Signal command */
  else if( StrOp.equals( NodeOp.getName(node), wSignal.name() ) ) {
    byte* out = allocMem(32);
    int module = wSignal.getaddr(node);
    int port   = wSignal.getport1(node);
    int gate   = wSignal.getgate1(node);
    int aspect = wSignal.getaspect(node);
    if ( module == 0 )
      module = 1;

    Boolean dccoutput = StrOp.equals( wSignal.getprot( node ), wOutput.prot_N );
    if( port == 0 )    //fada used convert to address, port
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port -1 + (dccoutput?0x3800:0x3000);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Signal %d (%s) aspect=%d", (address - (dccoutput?0x37FF:0x2FFF) ), dccoutput?"dcc":"mm", aspect );
    __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, aspect, 1, 0, 0);
    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte* out = allocMem(32);
    int swtime = data->swtime / 10;
    int module = wOutput.getaddr( node );
    if ( module == 0 )
      module = 1;

    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    Boolean dccoutput = StrOp.equals( wOutput.getprot( node ), wOutput.prot_N );
    if( port == 0 )    //fada used convert to address, port
      AddrOp.fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port -1 + (dccoutput?0x3800:0x3000);

    if ( StrOp.equals( wOutput.getcmd( node ), wOutput.on )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) on", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, 1, swtime/256, swtime%256);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) off", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 8, address, gate, 0, swtime/256, swtime%256);
    }

    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Loco command */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte* out = allocMem(32);
    byte* out2 = allocMem(32);
    byte* out3 = allocMem(32);
    Boolean sw     = wLoc.issw( node );
    const char* id = wLoc.getid( node );
    int addr       = wLoc.getaddr( node );
    int fn         = wLoc.isfn( node );
    int addroffset = 0;
    int dir        = 2 - wLoc.isdir( node );
    /* for cs2 1 is forwards, 2 is backwards, from server 1 = forwards, 0 is backwards */
    int speed      = 0;
    int speed1     = 0;
    int speed2     = 0;
    char prot[32]  = {'\0'};

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      StrOp.copy(prot,"dcc");
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) || StrOp.equals( wLoc.getprot( node ), wLoc.prot_F ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      StrOp.copy(prot,"mfx");
    }
    else {
      addroffset = 0x0000;
      /* MM loc address range start */
      StrOp.copy(prot,"mm");
    }

    long address = addr + addroffset;

    if( wLoc.getV( node ) != -1 ) {
      /* cs2 ranges all speeds from 0 - 1000 regardless of number of steps */
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = wLoc.getV( node ) * 10;
      else if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_step ) ) {
        float fspeed = 1000.0 / (float)wLoc.getspcnt( node );
        fspeed *= wLoc.getV( node );
        speed = (int)fspeed;
      }
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 1000) / wLoc.getV_max( node );
      speed1 = (speed & 0xFF00) >>8;
      speed2 = speed & 0x00FF;
    }

    if (sw) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %s %d (%s) %s", id, addr, prot, (dir==1)?"forwards":"backwards" );
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0, 0, 0);
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2, 0, 0);
      /* when changing direction cs2 set speed to 0 internally, so after direction change also send speed */
    } else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %s %d (%s) speed=%d(%d,%d) %s", id, addr, prot, speed, speed1, speed2, (dir==1)?"forwards":"backwards");
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2, 0, 0);
      /* also send direction to prevent going wrong way when user has changed direction on the cs2 */
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0, 0, 0);
    }
    /* send direction first, speed as second, because of CS2 setting speed to 0 on dir change */
    ThreadOp.post( data->writer, (obj)out );
    ThreadOp.post( data->writer, (obj)out2 );
    __setSysMsg(out3, 0, CMD_LOCO_FUNCTION , False, 6, address, 0, fn, 0, 0);
    ThreadOp.post( data->writer, (obj)out3 );
    return rsp;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    byte* out = allocMem(32);
    int fnchanged  = wFunCmd.getfnchanged(node);
    const char* id = wFunCmd.getid( node );
    int addr       = wFunCmd.getaddr( node );
    int addroffset = 0;
    char prot[32]  = {'\0'};

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      StrOp.copy(prot,"dcc");
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) || StrOp.equals( wLoc.getprot( node ), wLoc.prot_F ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      StrOp.copy(prot,"mfx");
    }
    else {
      addroffset = 0x0000;
      /* MM loc address range start */
      StrOp.copy(prot,"mm");
    }
    long address = addr + addroffset;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco %s %d (%s) function f%d to %s", id, addr, prot, fnchanged, __getFunctionState(node, fnchanged)?"on":"off" );

    if( fnchanged != -1 ) {
      __setSysMsg(out, 0, CMD_LOCO_FUNCTION , False, 6, address, fnchanged, __getFunctionState(node, fnchanged), 0, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom(node);
    int cv = wProgram.getcv( node );
    int value = wProgram.getvalue( node );
    int addr = wProgram.getaddr( node );
    int addroffset = 0;

    if( wProgram.getidentifier(node) != NULL && StrOp.len(wProgram.getidentifier(node))  > 0 ) {
      addr = atoi(wProgram.getidentifier(node));
    }

    if( StrOp.equals( wProgram.getstrval1( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
    }
    else if( StrOp.equals( wProgram.getstrval1( node ), wLoc.prot_P ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
    }
    addr += addroffset;

    if( wProgram.getcmd( node ) == wProgram.get ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d on %s for loco %d...", cv, pom?"POM":"PT", addr );
      __setSysMsg(out, 0, CMD_LOCO_READ_CONFIG , False, 7, addr, cv/256, cv%256, 1, 0);
      ThreadOp.post( data->writer, (obj)out );
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      byte* out = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d to %d on %s for loco %d...", cv, value, pom?"POM":"PT", addr );
      __setSysMsg(out, 0, CMD_LOCO_WRITE_CONFIG , False, 8, addr, cv/256, cv%256, value, pom?0x80:0x00);
      ThreadOp.post( data->writer, (obj)out );
    }
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone( node ), TRCLEVEL_INFO );

  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMCS2Data data = Data(inst);
  iONode rsp = NULL;
  if( cmd != NULL ) {
    if( StrOp.equals( NodeOp.getName( cmd ), wLocList.name() ) ) {
      data->locolist = cmd;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "locolist received" );
    }
    else {
      rsp = __translate( (iOMCS2)inst, cmd );
      /* Cleanup Node1 */
      cmd->base.del(cmd);
    }
  }

  return rsp;
}


static void _halt( obj inst, Boolean poweroff, Boolean shutdown ) {
  iOMCS2Data data = Data(inst);
  if( poweroff ) {
    byte* out = allocMem(32);
    __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP, 0, 0, 0);
    ThreadOp.post( data->writer, (obj)out );
    ThreadOp.sleep(200);
  }
  data->run = False;

  if( data->conOK && wDigInt.isasciiprotocol( data->ini ) && data->initASCII ) {
    ThreadOp.sleep(100);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (C) Close CAN channel" );
    SerialOp.write( data->serial, "C\r", 2 );
  }

  /* wait for all threads... */
  ThreadOp.sleep(1000);

  return;
}


static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMCS2Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}


static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOMCS2Data data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


static void __genHash() {
  rrHash = rand() % 0x8000 + 0x2000;
  rrHash &= 0xFF7F;
  rrHash |= 0x0300;
}


static void __feedbackMCS2Reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  ThreadOp.sleep( 1000 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 feedbackpoll started, polling %d S88 units", data->fbmod );
  data->sensor = True;
  do {
      ThreadOp.sleep( 250 );
    __SoD(mcs2);

    if( wDigInt.getprotver( data->ini ) >= 2 ) {
      /* Just poll once for Start of Day. */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Just poll once for Start of Day. V2" );
      break;
    }

  } while( data->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "feedbackpoll ended." );
}


static void __evaluateMCS2S88( iOMCS2Data mcs2, byte* in, unsigned char* prev ) {
  int s88base = in[9] * 16;
  int n = 0;
  int addr = 0;
  int state = 0;
  int t = 0;
  for( t = 0; t < 2; t++) {
    for( n = 0; n < 8; n++ ) {
      addr = s88base + n + 1 + (t * 8);
      state = (in[11 - t] & (0x01 << n)) ? 1:0;
      /* cs2 uses big endian, in11 contains lower 8 inputs, in10 the higher 8 */
      if( prev[addr - 1] != state ) {
        /* this feedback changed state since previous poll */
        prev[addr - 1] = state;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %d", addr, state );
        {
          /* inform listener: Node */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr );
          wFeedback.setstate( nodeC, state?True:False );
          if( mcs2->iid != NULL )
            wFeedback.setiid( nodeC, mcs2->iid );
          mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }

  if(!mcs2->sensor) {
    mcs2->sensor = True;
    __reportState(mcs2);
  }
}


/* 00 23 67 78 08 00 00 00 0B 00 01 00 00 */
static void __evaluateSensorEvent( iOMCS2Data mcs2, byte* in ) {
  int bus    = in[5] * 256 + in[6];
  int addr   = in[7] * 256 + in[8];
  int state = in[10];
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d:%d state=%d", bus, addr, state );
  wFeedback.setbus( nodeC, bus );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  if( mcs2->iid != NULL )
    wFeedback.setiid( nodeC, mcs2->iid );
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );

  if(!mcs2->sensor) {
    mcs2->sensor = True;
    __reportState(mcs2);
  }

}


static void __evaluateMCS2Loc( iOMCS2Data mcs2, byte* in ) {
  int addr  = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  int speed = (in[9] << 8) + in[10];
  int dir   = in[9];
  iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wLoc.setiid( nodeC, mcs2->iid );
  wLoc.setaddr( nodeC, addr );
  if( in[1] == 0x0A ) {
    /* loc command was a direction command.
       The cs2 sets loc speed to 0 internally on dir change and does not broadcast this. Since the command was not
       send by Rocrail, we have to assume that speed of loc is now set to zero. */
    wLoc.setV_raw( nodeC, 0 );
    wLoc.setV_rawMax( nodeC, 1000 );
    wLoc.setdir( nodeC, dir==1 );
    wLoc.setthrottleid( nodeC, "MS2" );
    /* 1 means forwards, 2 means reverse in cs2 message, in Rocrail true=forward, false=reverse */
//    wLoc.setcmd( nodeC, wLoc.velocity );
    wLoc.setcmd( nodeC, wLoc.direction );
  } else {
    /* loc message was speed message */
    wLoc.setV_raw( nodeC, speed );
    wLoc.setV_rawMax( nodeC, 1000 );
    /* all cs2 speeds are on a scale of 0-1000, regardless of actual locdecoder speedsteps */
    wLoc.setcmd( nodeC, wLoc.velocity );
    wLoc.setthrottleid( nodeC, "MS2" );
  }
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __evaluateMCS2Function( iOMCS2Data mcs2, byte* in ) {
  int addr     = 0;
  int function = in[9];
  int state    = in[10];
  addr = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  if( function <= 28 ) {
    /* cs2 supports 32 functions, Rocrail 28 */
    iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( mcs2->iid != NULL )
      wLoc.setiid( nodeC, mcs2->iid );
    wFunCmd.setaddr( nodeC, addr);
    wLoc.setthrottleid( nodeC, "MS2" );
    wFunCmd.setfnchanged( nodeC, function);
    wLoc.setcmd( nodeC, wLoc.function );
    switch ( function ) {
      case 0 : wFunCmd.setf0( nodeC, ( state ? True : False)); break;
      case 1 : wFunCmd.setf1( nodeC, ( state ? True : False)); break;
      case 2 : wFunCmd.setf2( nodeC, ( state ? True : False)); break;
      case 3 : wFunCmd.setf3( nodeC, ( state ? True : False)); break;
      case 4 : wFunCmd.setf4( nodeC, ( state ? True : False)); break;
      case 5 : wFunCmd.setf5( nodeC, ( state ? True : False)); break;
      case 6 : wFunCmd.setf6( nodeC, ( state ? True : False)); break;
      case 7 : wFunCmd.setf7( nodeC, ( state ? True : False)); break;
      case 8 : wFunCmd.setf8( nodeC, ( state ? True : False)); break;
      case 9 : wFunCmd.setf9( nodeC, ( state ? True : False)); break;
      case 10 : wFunCmd.setf10( nodeC, ( state ? True : False)); break;
      case 11 : wFunCmd.setf11( nodeC, ( state ? True : False)); break;
      case 12 : wFunCmd.setf12( nodeC, ( state ? True : False)); break;
      case 13 : wFunCmd.setf13( nodeC, ( state ? True : False)); break;
      case 14 : wFunCmd.setf14( nodeC, ( state ? True : False)); break;
      case 15 : wFunCmd.setf15( nodeC, ( state ? True : False)); break;
      case 16 : wFunCmd.setf16( nodeC, ( state ? True : False)); break;
      case 17 : wFunCmd.setf17( nodeC, ( state ? True : False)); break;
      case 18 : wFunCmd.setf18( nodeC, ( state ? True : False)); break;
      case 19 : wFunCmd.setf19( nodeC, ( state ? True : False)); break;
      case 20 : wFunCmd.setf20( nodeC, ( state ? True : False)); break;
      case 21 : wFunCmd.setf21( nodeC, ( state ? True : False)); break;
      case 22 : wFunCmd.setf22( nodeC, ( state ? True : False)); break;
      case 23 : wFunCmd.setf23( nodeC, ( state ? True : False)); break;
      case 24 : wFunCmd.setf24( nodeC, ( state ? True : False)); break;
      case 25 : wFunCmd.setf25( nodeC, ( state ? True : False)); break;
      case 26 : wFunCmd.setf26( nodeC, ( state ? True : False)); break;
      case 27 : wFunCmd.setf27( nodeC, ( state ? True : False)); break;
      case 28 : wFunCmd.setf28( nodeC, ( state ? True : False)); break;
    }
    mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __reportState(iOMCS2Data data) {
  if( wDigInt.isreportstate(data->ini) && data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );

    wState.setpower( node, data->power );
    wState.settrackbus( node, data->power );
    wState.setsensorbus( node, data->sensor );
    wState.setaccessorybus( node, True );
    wState.setload( node, data->load );
    wState.setvolt( node, data->volt );
    wState.settemp( node, data->temp );
    wState.setshortcut(node, data->overload);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __setSwitchtime( iOMCS2Data data ) {
  byte* out = allocMem(32);
  __setSysMsg(out, 0, CMD_SYSTEM, False, 7, 0, CMD_SYSSUB_SWTIME, (data->swtime/10)/256, (data->swtime/10)%256, 0);
  ThreadOp.post( data->writer, (obj)out );
  data->swtimeset = True;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Switch command time set to: %dms", data->swtime );
}


static void __evaluateMCS2System( iOMCS2Data data, byte* in ) {
  int cmd = in[9];
  int addr4 = in[8];
  int addr3 = in[7];
  int addr2 = in[6];
  int addr1 = in[5];

  if ( (addr1 == 0) && (addr2 == 0) && (addr3 == 0) && (addr4 == 0) ) {
//    if (cmd == CMD_SYSSUB_STOP ) {
    if (cmd == CMD_SYSSUB_STOP && data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CS2 STOP" );
      data->power = False;
      __reportState(data);
    }
//    else if (cmd == CMD_SYSSUB_GO ) {
    else if (cmd == CMD_SYSSUB_GO && !data->power ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CS2 GO" );
      data->power = True;
      data->overload = False;
      __reportState(data);
    }
  }
  if( cmd == CMD_SYSSUB_OVERLOAD ) {
    data->overload = True;
    int uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Overload on UID: 0x%04X on interface 0x%s", uid, data->iid );
    __reportState(data);
  }
  if( cmd == CMD_SYSSUB_STATUS && data->gbUID != 0 ) {
    switch (in[10]){
    case 1:
      //Current in mA
      data->load = (int)((in[11] * 256 + in[12]) * (1000./824.));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Current I=0x%04X converted to %dmA for UID: 0x%04X", (in[11] * 256 + in[12]), data->load, data->gbUID );
    break;
    case 3:
      //Voltage in mV
      data->volt = (int)(((in[11] * 256 + in[12])* (1000./185.))+10000);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Voltage U=0x%04X converted to %dmV for UID: 0x%04X", (in[11] * 256 + in[12]), data->volt, data->gbUID );
    break;
    case 4:
      //Temperature in degree C
      data->temp = (int)((in[11] * 256 + in[12]) * (74./202.));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Temperature t=0x%04X converted to %d degree C for UID: 0x%04X", (in[11] * 256 + in[12]), data->temp, data->gbUID );
    break;
    }
    __reportState(data);
  }
}


static void __evaluateMCS2PingCmd( iOMCS2Data data, byte* in ) {
  if( data->mcs2guiUID == 0 && data->ms2UID == 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "User interface detected, ping send to identify" );
    byte  buffer[32];
    buffer[0]  = 0;
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, False, 0, buffer) );
  }
}


static void __evaluateMCS2Ping( iOMCS2Data data, byte* in ) {
//  const int sysgerken = 0xFF10;
//00000000: 00 31 23 09 08 47 43 4E 61 01 27 00 10 
  int cstype = in[11] * 256 + in[12];
  if( in[1] == ( CAN_ID_PING | BIT_RESPONSE ) ) {
    cstype &= 0xFFF0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Evaluate Ping for type: 0x%04X", cstype );
    if( cstype == 0x0010 ) {
      data->gbUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Gleisbox UID: 0x%04X stored", data->gbUID );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Gleisbox firmware version = %d.%d", in[9], in[10] );
    }
    else if( cstype == 0x0000 ) {
      data->mcs2gfpUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GFP UID: 0x%04X stored", data->mcs2gfpUID );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GFP firmware version = %d.%d", in[9], in[10] );
      if( data->gbUID != 0 ) {
        /* error */
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ERROR: Gleisbox and CS2 found on CAN bus, Gleisbox 0x%04X cleared", data->gbUID );
        data->gbUID = 0;
      }
    }
    else if( cstype == 0xFFF0 ) {
      data->mcs2guiUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GUI UID: 0x%04X stored", data->mcs2guiUID );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS2-GUI firmware version = %d.%d", in[9], in[10] );
    }
    /* despite the CAN documentation the "Geraetekennung" of a MS2 is 0x0032 */
    else if( cstype == 0x0030 ) {
      data->ms2UID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MS-2 UID: 0x%04X stored", data->ms2UID );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MS-2 firmware version = %d.%d", in[9], in[10] );
    }

    if( !data->swtimeset && data->mcs2gfpUID !=0 )
        __setSwitchtime(data);
  }
  int rcvHash = (in[2] << 8) + in[3];

  if( rcvHash == rrHash ) {
    __genHash();      
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Duplicate CANbus address detect for hash: 0x%04X, new hash generated: 0x%04X", rcvHash, rrHash );
  }
return;
  /*  if requested announce ourself as a CANbus member, nice but for now it only generates troubles */
/*  byte  buffer[32];
  buffer[0]  = 'R';
  buffer[1]  = 'c';
  buffer[2]  = 'r';
  buffer[3]  = 'l';
  buffer[4]  = (revisionnr / 100) & 0xFF;
  buffer[5]  = (revisionnr % 100) & 0xFF;
  buffer[6]  = (sysgerken / 256) & 0xFF;
  buffer[7]  = (sysgerken % 256) & 0xFF;
  ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, True, 8, buffer) );
*/
}

static void __evaluateMCS2Switch( iOMCS2Data mcs2, byte* in ) {
  int addr1 = in[7] & 0x0F;
  /* mask left nibble of high byte, this is not part of the actual address (always 0x3 for accessory) */
  int addr2 = in[8];
  int state = in[9];
  int port  = 0;
  int addr  = 0;

  if( addr1 > 7 )
    addr1 = addr1 - 8;
    /* address range start 0x3000 for MM, range start for DCC is 0x3800, so MM 0x3000 or DCC 0x3800 is Rocrail address 1, port 1 */
  addr2 = addr2 + (addr1 << 8) + 1;
  AddrOp.fromPADA( addr2, &addr, &port );

  iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wSwitch.setiid( nodeC, mcs2->iid );
  wSwitch.setaddr1( nodeC, ( addr ) );
  wSwitch.setport1( nodeC, ( port ) );
  wSwitch.setstate( nodeC, (state == 1)?"straight":"turnout" );
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}


static Boolean __findLocinList(iOMCS2Data data) {
  if( data->locolist != NULL ) {
    iONode loProps = wLocList.getlc( data->locolist );

    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );

      byte addrFormat = 0x00;
//      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "comparing %s in locolist name: %s, registered sid: %d, requested sid: %d", data->id, loName, wLoc.getaddr(loProps), data->sid);
      
      if( StrOp.equals( wLoc.prot_F, wLoc.getprot( loProps ) )  || StrOp.equals( wLoc.prot_P, wLoc.getprot( loProps ) ) ) {
        /* mfx */
        addrFormat = 0x40;
      }
      if( (wLoc.getaddr(loProps) == data->sid) && (addrFormat == 0x40) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx discovered loc already in list, sid: %d, name: %s, iid: %s, preventing duplicate.", data->sid, loName, data->iid);
        return True;
      }
      
      if( StrOp.equals( data->id, loName ) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "loc id already in list, for name: %s, registered sid: %d, requested sid: %d, iid: %s", loName, wLoc.getaddr(loProps), data->sid, data->iid);
        data->sid = wLoc.getaddr(loProps);
        return True;
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
  return False;
}


static Boolean __findSidinLocList(iOMCS2Data data) {
int addr;
  if( data->locolist != NULL ) {
    iONode loProps = wLocList.getlc( data->locolist );

    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );
      if( wLoc.getaddr(loProps) == data->sid ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "locolist requested sid: %d found, name: %s", wLoc.getaddr(loProps), loName);
        return True;
      }
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
  return False;
}


static Boolean __findSidinProductList(iOMCS2Data data) {
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getsid(loco) == data->sid ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "productlist requested sid: %d found, name: %s", wProduct.getsid(loco), wProduct.getdesc(loco));
      return True;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return False;
}


static iONode __getUID(iOMCS2Data data) {
  int sid;
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getpid(loco) == data->uid ) {
      return loco;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return NULL;
}


static int __countUID(iOMCS2Data data) {
  int cnt = 0;
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    cnt++;
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return cnt;
}


static iONode __getBySID(iOMCS2Data data, int sid) {
  iONode loco = wMCS2.getproduct(data->mcs2ini);

  while( loco != NULL ) {
    if( wProduct.getsid(loco) == sid ) {
      return loco;
    }
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  return NULL;
}


static byte __getnewSID(iOMCS2Data data) {
  if( data->sid < 3 )
    data->sid = 3;
  while( __findSidinLocList(data) || __findSidinProductList(data) ) {
    data->sid++;
  }
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "new sid: %d", data->sid);
  return data->sid;
}


static void __clearRegMfxVar(iOMCS2Data data) {
  int i;
  data->sid = 0;
  data->uid = 0;
  data->xS2handling = 0;
  data->mfxDetectInProgress = False;
  for( i=0; i<17 ; i++ ) {
    idname[i] = '\0';
  }
}


static Boolean __checkForValidName(iOMCS2Data data) {
  int i;
  char adaptname[17];
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "checking for valid name: %s", idname);
  if( StrOp.len(idname) > 0 ) {
    for( i=0 ; i < StrOp.len(idname) ; i++ ) {
      if( idname[i] < ' '  || idname[i] > '~' || i == 17) {
        StrOp.fmtb( adaptname, "mfx-%d", data->sid);
        StrOp.copy( idname, adaptname );
        TraceOp.trc(name, TRCLEVEL_ERROR, __LINE__, 9999, "Invalid name, adapting loconame to: %s", idname);
        return False; 
      } 
    }
  }
  else {
    StrOp.fmtb( adaptname, "mfx-%d", data->sid);
    StrOp.copy( idname, adaptname );
    TraceOp.trc(name, TRCLEVEL_ERROR, __LINE__, 9999, "zero length, adapted loconame: %s", idname);
    return False;
  }
  return True;
}


static void __reqFullName(iOMCS2Data data, byte* in) {
  byte buffer[32];
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = in[9] | 0x40; 
  buffer[3] = in[10];
  buffer[4] = 0x04; // index 1
  buffer[5] = 0x03; //cv3
  buffer[6] = 0x10;
  ThreadOp.post(data->writer, (obj) __makeMsg(0, CMD_LOCO_READ_CONFIG, False, 7, buffer));
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Requesting full name for UID=0x%08X, address %d", data->uid, in[10]);
}

static void __registerMCS2DetectedMfxLoco(iOMCS2Data data) {
  char identifier[32] = {'\0'};
  iONode loco = __getUID(data);
  if( !__findLocinList(data) ) {
    StrOp.copy( idname, wProduct.getdesc(loco) );            

    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Registering request id: %s, sid: %d, UID: 0x%08X,  controllerpath: %s", data->id, data->sid, data->uid, data->iid);

    if( loco == NULL ) {
      loco = NodeOp.inst(wProduct.name(), data->mcs2ini, ELEMENT_NODE);
      NodeOp.addChild(data->mcs2ini, loco);
      wProduct.setpid(loco, data->uid);
      if( data->ms2UID == 0 && data->mcs2guiUID == 0 && data->sid == 1 ) {
        wProduct.setsid(loco, __getnewSID( data));
      }
      wProduct.setdesc(loco, data->id);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "product added sid: %d, UID: 0x%08X, id: %s",  wProduct.getsid(loco), data->uid, data->id);
    }
    wProduct.setcid(loco,1);

    iONode lc = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
    wLoc.setaddr(lc, wProduct.getsid(loco));
    StrOp.fmtb(identifier, "%u", data->uid);
    wLoc.setidentifier(lc, identifier);
    wLoc.setid(lc, data->id);
    wLoc.setprot(lc, wLoc.prot_F); // MFX
    wLoc.setcmd(lc, wLoc.discover);
    if (data->iid != NULL)
      wLoc.setiid(lc, data->iid);
    data->listenerFun(data->listenerObj, lc, TRCLEVEL_INFO);
  }
  else {
    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X found in locolist, marked as registered", data->id, data->sid, data->uid);
    wProduct.setcid(loco,1);
  }
  __clearRegMfxVar(data);
}


static void __evaluateMCS2ReadConfig(iOMCS2Data data, byte* in) {
  int index =  (in[9] & 0xFC) >> 2;
  int cv    =  (in[9] & 0x03) * 256 + in[10];
  int addr  =  ((in[7] & 0x0F) << 8) + in[8];
  int value =  in[11];
  int i     =  0;
  int dlc   =  in[4];

  if( dlc == 7 ) {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, index: %d, cv: %d, value: 0x%02X", addr, index, cv, value);
    if( (index != 0) && (cv == 3) ) {

      if( index == 1 && idname != NULL && StrOp.len(idname) > 0 ) {
        data->mfxDetectInProgress = True;
        for( i = 0; i < StrOp.len(idname) ; i++ ) {
          idname[i] = '\0';
        }
      }

      idname[index - 1] = in[11];
      if( index == 0x10 ) {
        data->sid = addr;
        __checkForValidName(data);
  	/* in case of a xS2 controlled procedure, the sid adress changed from 1 to x in the end of the registering procedure */ 
        iONode loco = __getBySID(data, addr);
        wProduct.setdesc(loco, data->id);
        
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "catched name sid: %d, UID: 0x%08X, loc %s, iid: %s", addr, wProduct.getpid(loco), data->id, data->iid);
        if( addr > 1 ) {   
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "register from readconfig name sid: %d, UID: 0x%08X, loc %s, iid: %s", addr, wProduct.getpid(loco), data->id, data->iid);
          data->uid = wProduct.getpid(loco);
          __registerMCS2DetectedMfxLoco(data);
        }
        if( data->ms2UID == 0 && data->mcs2guiUID == 0 && (wProduct.getsid(loco) == 1 || data->sid == 1) ) {
          if( __findLocinList(data)) {
            wProduct.setsid(loco, data->sid);
          } 
          else {
            wProduct.setsid(loco, __getnewSID( data));
          }
          byte  buffer[32];
          buffer[0]  = (wProduct.getpid(loco) & 0xFF000000) >> 24;;
          buffer[1]  = (wProduct.getpid(loco) & 0x00FF0000) >> 16;
          buffer[2]  = (wProduct.getpid(loco) & 0x0000FF00) >> 8;
          buffer[3]  = (wProduct.getpid(loco) & 0x000000FF);
          buffer[4]  = (wProduct.getsid(loco) / 256) & 0xFF;
          buffer[5]  = (wProduct.getsid(loco) % 256) & 0xFF;
          ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "bind posted for UID: 0x%08X, sid: %d", wProduct.getpid(loco), wProduct.getsid(loco) );
        }
      }
    }
    
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

    cv = in[9]*256 + in[10];
    wProgram.setcv( node, cv );
    wProgram.setvalue( node, value );
    wProgram.setcmd( node, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

  }
}


static void __evaluateMCS2WriteConfig( iOMCS2Data data, byte* in ) {
  int dlc = in[4];
  int index =  (in[9] & 0xFC) >> 2;
  int cv    =  (in[9] & 0x03) * 256 + in[10];
  int addr  =  ((in[7] & 0x0F) << 8) + in[8];
  int value =  in[11];
  int i     =  0;
  if( dlc == 8 ) {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "index: %d, cv: %d, value: 0x%02X", index, cv, value);
    if( (index != 0) && (cv == 3) ) {

      if( index == 1 && StrOp.len(idname) > 0 ) {
        data->mfxDetectInProgress = True;
        for( i = 0; i < StrOp.len(idname) ; i++ ) {
          idname[i] = '\0';
        }
      }

      idname[index - 1] = in[11];
      if( in[11] == 0 ) {
  	/* in case of a xS2 controlled procedure, 0 means end of name */ 
        iONode loco = __getBySID(data, addr);
        data->uid = wProduct.getpid(loco);
        data->sid = wProduct.getsid(loco);
        wProduct.setdesc(loco, data->id);
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "sid: %d, UID: 0x%08X, loc %s, iid: %s", data->sid, data->uid, data->id, data->iid);
        TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Name changed: %s, address: %d, UID: 0x%08X, valid at restart", data->id, wProduct.getsid(loco), wProduct.getpid(loco));
        if( __checkForValidName(data) )
            __registerMCS2DetectedMfxLoco(data);
      }
    }
    else {
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      cv = in[9]*256 + in[10];

      wProgram.setcv( node, cv );
      wProgram.setvalue( node, value );
      wProgram.setcmd( node, wProgram.datarsp );
      if( data->iid != NULL )
          wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
  }
}

static void __evaluateMCS2Verify( iOMCS2Data mcs2, byte* in ) {
/* 00064711 6 FF FA 8C 43 00 05 */
  mcs2->sid  = (in[9] << 8) + in[10];

  if( wMCS2.isdiscovery(mcs2->mcs2ini) ) {
    if( mcs2->sid > 0 ) {
      mcs2->uid =  (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
      iONode loco = __getUID(mcs2);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind verified succesfull for UID=0x%08X to address %d", mcs2->uid, mcs2->sid);

      if( mcs2->ms2UID == 0 && mcs2->mcs2guiUID == 0 && mcs2->sid == 1 && wProduct.getcid(loco) == 0 ) {
        __reqFullName(mcs2, in);
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "full name requested for sid: %d", mcs2->sid);
      }
      /* sid 1 is used during registering */
      if( (mcs2->sid > 1)  && wProduct.getcid(loco) == 0 ){
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Registering by verify UID=0x%08X, address %d", mcs2->uid, mcs2->sid);
        StrOp.copy( idname, wProduct.getdesc(loco) );            
        if( !__checkForValidName(mcs2)) {
          __reqFullName(mcs2, in);
        }
        else { 
          __registerMCS2DetectedMfxLoco(mcs2);
        }
      }
      else {
        if( wProduct.getcid(loco) == 1 ){
          if( mcs2->sid > 1 ) {
            TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X already handled or created", wProduct.getdesc(loco), mcs2->sid, mcs2->uid);
          }
          else {
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "ID: %s, sid: %d,  UID: %08X already handled or created", wProduct.getdesc(loco), mcs2->sid, mcs2->uid);
          }
        }
        if( mcs2->sid > 1 )
            __clearRegMfxVar(mcs2);
      }
    }
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Verify failed for UID=0x%08X", mcs2->uid);
      __clearRegMfxVar(mcs2);
    }
  }
  else {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Verify for UID=0x%08X discarded (bind is not activated)", mcs2->uid);
  }
}


static void __evaluateMCS2Bind( iOMCS2Data mcs2, byte* in ) {
  mcs2->uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
  mcs2->sid = (in[9] << 8) + in[10];
  if ( wMCS2.isbind(mcs2->mcs2ini) || wMCS2.isdiscovery(mcs2->mcs2ini) ) {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx bind invoked for UID=0x%08X, for sid: %d", mcs2->uid, mcs2->sid);
    mcs2->mfxDetectInProgress = True;
    iONode loco = __getUID(mcs2);
    if( loco == NULL ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X creating product registered to address %d", mcs2->uid, mcs2->sid);
      loco = NodeOp.inst(wProduct.name(), mcs2->mcs2ini, ELEMENT_NODE);
      NodeOp.addChild(mcs2->mcs2ini, loco);
      wProduct.setpid(loco, mcs2->uid);
      wProduct.setsid(loco, mcs2->sid);
    }  
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X (%s) product already registered to address %d", mcs2->uid, wProduct.getdesc(loco), wProduct.getsid(loco));
      if( wProduct.getdesc(loco) != NULL ) { 
        StrOp.copy( idname, wProduct.getdesc(loco) );
      }
    }       

    if( wProduct.getsid(loco) != mcs2->sid && mcs2->sid > 1 ) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind UID=0x%08X modifying sid %d to new sid %d", mcs2->uid, wProduct.getsid(loco), mcs2->sid);
      wProduct.setsid(loco, mcs2->sid);
    }
    if( wMCS2.isdiscovery(mcs2->mcs2ini) ) {    
      if( (mcs2->ms2UID == 0) && (mcs2->mcs2guiUID == 0) ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind acknowledged, requesting Verify UID=0x%08X on sid: %d", mcs2->uid, mcs2->sid);
        byte  buffer[32];
        buffer[0]  = in[5];
        buffer[1]  = in[6];
        buffer[2]  = in[7];
        buffer[3]  = in[8];
        buffer[4] = (mcs2->sid / 256) & 0xFF;
        buffer[5] = (mcs2->sid % 256) & 0xFF;
        ThreadOp.post(mcs2->writer, (obj) __makeMsg(0, CMD_LOCO_VERIFY, False, 6, buffer));
      }
      else {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind listening (no action) for CS2/MS2 controlled procedure. UID: 0x%08X on sid: %d", mcs2->uid, mcs2->sid);
      }
    }
    else {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind only enabled, ending Bind for UID=0x%08X (discovery is not activated)", mcs2->uid);
      if( !__findSidinLocList(mcs2) ) {
        __checkForValidName(mcs2);
        __registerMCS2DetectedMfxLoco(mcs2);
      }
      else {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Product UID=0x%08X already in locolist, not added", mcs2->uid);
        __clearRegMfxVar(mcs2);
      }
    }
  }
  else {
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind for UID=0x%08X discarded (bind & discovery is not activated)", mcs2->uid);
  }
}


static void __evaluateMCS2Discovery( iOMCS2Data mcs2, byte* in ) {
/*
  T000303005FFE405E920<\r>
*/
  int dlc = in[4];
  if( dlc == 0 ){
    mcs2->mfxDetectInProgress = False;
    TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "Discovery completed, ready for operation");
  }

  if( dlc == 6 && (in[5] & in[6] & in[7] & in[8] == 0xFF) && in[9] == 0x03 && wMCS2.isdiscovery(mcs2->mcs2ini) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start mfx detect by gfp software detected" );
    mcs2->mfxDetectInProgress = True;
  }

  if( dlc == 5 && mcs2->uid != (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8] ) {
    mcs2->uid = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
    int protocoltype = in[9];
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery requested for UID=0x%08X", mcs2->uid);
    if( wMCS2.isdiscovery(mcs2->mcs2ini) ) {
      mcs2->mfxDetectInProgress = True;
      iONode loco = __getUID(mcs2);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Registration status for UID=0x%08X (%s) %d", mcs2->uid, wProduct.getdesc(loco), wProduct.getcid(loco));
      if ( loco != NULL ) {
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X (%s) already registered to address %d", mcs2->uid, wProduct.getdesc(loco), wProduct.getsid(loco));
        mcs2->sid = wProduct.getsid(loco);
        if( wProduct.getdesc(loco) != NULL ) { 
          StrOp.copy( idname, wProduct.getdesc(loco) );
        }
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X name in structure: %s", wProduct.getpid(loco), mcs2->id);
      }
      if( mcs2->ms2UID == 0 && mcs2->mcs2guiUID == 0) {
        if ( loco == NULL || StrOp.len(idname) == 0 ) {
          mcs2->sid = 1;
          TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery UID=0x%08X to temporary address %d", mcs2->uid, mcs2->sid);
          wProduct.setsid(loco, 1);
        }
        byte  buffer[32];
        buffer[0]  = in[5];
        buffer[1]  = in[6];
        buffer[2]  = in[7];
        buffer[3]  = in[8];
        buffer[4]  = (mcs2->sid / 256) & 0xFF;
        buffer[5]  = (mcs2->sid % 256) & 0xFF;

        ThreadOp.post( mcs2->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Bind requested by Rocrail for sid %d", mcs2->sid);
      }
      else {
            TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Discovered UID=0x%08X handled by CS2/MS2", mcs2->uid);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Discovery discarded for UID=0x%08X (discovery is not activated)", mcs2->uid );
    }
  }
}

static void __evaluateMCS2CANBootBound( iOMCS2Data data, byte* in ) {
  int cstype = in[11] * 256 + in[12];
  int highword = 0;
  int lowword =0;
  int controlhash =0;
  int rcvHash =0;

  cstype &= 0xFFF0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Evaluate CAN boot bound for type: 0x%04X", cstype );
  if( cstype == 0x0010 && data->ms2UID == 0 && data->mcs2gfpUID == 0 && data->gbUID == 0 ) {
      rcvHash = (in[2] << 8) + in[3];
      highword = (in[5] << 8) + in[6];
      lowword = (in[7] << 8) + in[8];
      controlhash = ( highword ^ lowword ) | 0x0300;   /* this is the way according to the documentation S.6 */
      lowword ^= highword;                             /* this is the way it is implemented in Gleisbox V1.39 */
      if( controlhash == rcvHash || lowword == rcvHash ) {
        data->gbUID = (in[5] << 24) + (in[6] << 16) + (in[7] << 8) + in[8];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Gleisbox found with UID: 0x%08X", data->gbUID );
        byte  buffer[32];
        buffer[0]  = 0;
        buffer[1]  = 0;
        buffer[2]  = 0;
        buffer[3]  = 0;
        buffer[4]  = 0x11;
        buffer[5]  = 0;
        buffer[6]  = 0;
        buffer[7]  = 0;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Sending boot with current version command to standalone Gleisbox" );
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_CAN_BOOT_BOUND, False, 5, buffer) );
//        data->power = False;
        ThreadOp.sleep(2000);
        buffer[4]  = 0;
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, False, 0, buffer) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Invalid packet received on boot bound request" );
      }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN Boot not handled, unknown type: 0x%04X", cstype );
  }
}

static void __evaluateCCSwitch( iOMCS2Data mcs2, byte* in ) {
  int addr1 = in[7] & 0x0F;
  /* mask left nibble of high byte, this is not part of the actual address (always 0x3 for accessory) */
  int addr2 = in[8];
  int state = in[9];
  int port  = 0;
  int addr  = 0;

  if( addr1 > 7 )
    addr1 = addr1 - 8;
    /* address range start 0x3000 for MM, range start for DCC is 0x3800, so MM 0x3000 or DCC 0x3800 is Rocrail address 1, port 1 */
  addr2 = addr2 + (addr1 << 8) + 1;
  AddrOp.fromPADA( addr2, &addr, &port );

  if( state == 0xFF) {
    /* error */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "CC Switch %d.%d report: ERROR", addr, port );
  }
  else {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    if( mcs2->iid != NULL )
      wSwitch.setiid( nodeC, mcs2->iid );
    wSwitch.setaddr1( nodeC, ( addr ) );
    wSwitch.setport1( nodeC, ( port ) );
    wSwitch.setstate( nodeC, (state == 0xFE)?"turnout":"straight" );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CC Switch %d.%d report: 0x%02X", addr, port, state );
    mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static byte __HEXA2Byte( const char* s ) {
  char val[3] = {0};
  val[0] = s[0];
  val[1] = s[1];
  val[2] = '\0';
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "HEXA=[%s]", val );
  return (unsigned char)(strtol( val, NULL, 16)&0xFF);
}


static void __Byte2HEXA( char* s, byte b ) {
  static char cHex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  s[0] = cHex[(b&0xF0)>>4 ];
  s[1] = cHex[ b&0x0F     ];
}


static Boolean __convertASCII2Bin( char* inASCII, byte* in) {
  MemOp.set(in, 0, 32);
  if( inASCII[0] != 'T') {
    StrOp.replaceAll(inASCII, '\r', ' ');
    TraceOp.trc( name, inASCII[0] == 'Z' ? TRCLEVEL_DEBUG:TRCLEVEL_MONITOR, __LINE__, 9999, "ASCII message: %s", inASCII );
    in[1] = 0xFF;
    return False;
  }
  else {
    int i = 0;
    /*
     * Tiiiiiiii l dd..[CR]
     * T0000030050000000001[CR] */
    in[4] = inASCII[9] - '0';
    in[0] = __HEXA2Byte(inASCII+1);
    in[1] = __HEXA2Byte(inASCII+3);
    in[2] = __HEXA2Byte(inASCII+5);
    in[3] = __HEXA2Byte(inASCII+7);

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "ASCII to bin: len=%d", in[4] );
    for( i = 0; i < in[4]; i++ ) {
     in[5+i] = __HEXA2Byte(inASCII+10+(i*2));
    }

    return True;
  }
  return False;
}


static int __convertBin2ASCII( const byte* out, char* outASCII) {
  int i = 0;
  /*
   * Tiiiiiiii l dd..[CR]
   * T00360301 5 00 00 00 00 11 [CR] */
  outASCII[0] = 'T';
  outASCII[1] = '\0';

  __Byte2HEXA(outASCII+1, out[0]);
  __Byte2HEXA(outASCII+3, out[1]);
  __Byte2HEXA(outASCII+5, out[2]);
  __Byte2HEXA(outASCII+7, out[3]);
  outASCII[9] = out[4] + '0';

  for(i = 0; i < out[4]; i++ ) {
    __Byte2HEXA(outASCII+10+(i*2), out[5+i]);
  }
  outASCII[10+(i*2)] = '\r';
  outASCII[10+(i*2)+1] = '\0';

  return StrOp.len(outASCII);
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  byte in[32];
  int mod = 0;
  unsigned char store[1024];
  int retry = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader started." );

  for( mod = 0; mod < 1024; mod++) {
    store[mod] = 0;
    /* storage container for feedback states to check on changes */
  }

  data->id = idname;

  do {
    MemOp.set(in, 0, 32);
    if( data->udp ) {
      if( SocketOp.recvfrom( data->readUDP, (char*)in, 13, NULL, NULL ) <= 0 ) {
        SocketOp.base.del(data->readUDP);
        ThreadOp.sleep(1000);
        if( data->run ) {
          data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15730, False, True, False );
          SocketOp.bind(data->readUDP);
          continue;
        }
        else break;
      }
    }
    else {
      if( data->conOK ) {
        /* Init ASCII protocol if needed. */
        if( wDigInt.isasciiprotocol( data->ini ) && !data->initASCII ) {
          Boolean ok = True;
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (S5) 250 kBaud" );
            ok = SerialOp.write( data->serial, "S5\r", 3 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (O) Open CAN channel." );
            ok = SerialOp.write( data->serial, "O\r", 2 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }
          if(ok) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init ASCII: (V) Get hardware version." );
            ok = SerialOp.write( data->serial, "V\r", 2 );
            ThreadOp.sleep(500); /* sleep baby sleep */
          }

          if( ok ) {
            byte*  cmd   = allocMem(32);
            MemOp.set(cmd, 0, 32);
            cmd[0] = (0x360301 & 0xFF000000) >> 24;
            cmd[1] = (0x360301 & 0x00FF0000) >> 16;
            cmd[2] = (0x360301 & 0x0000FF00) >> 8;
            cmd[3] = (0x360301 & 0x000000FF);
            cmd[4] = 5;
            cmd[9] = 0x11;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "send magic ASCII" );
            ThreadOp.post( data->writer, (obj)cmd );
            data->initASCII = True;
          }
        }

        int available = SerialOp.available(data->serial);
        if( available > 0 ) {
          if( wDigInt.isasciiprotocol( data->ini ) ) {
            char inASCII[64] = {'\0'};
            Boolean CR = False;
            int idx = 0;
            while(SerialOp.available(data->serial) > 0 ) {
              if( SerialOp.read( data->serial, (char*)(inASCII+idx), 1 ) ) {
                if( SerialOp.available(data->serial) == 0 )
                  ThreadOp.sleep(10);
                idx++;
                inASCII[idx] = '\0';
                if( inASCII[idx-1] == '\r' ) {
                  CR = True;
                  break;
                }
              }
              else {
                break;
              }
            }
            if( CR ) {
              TraceOp.trc( name, inASCII[0] != 'T' ? TRCLEVEL_DEBUG:TRCLEVEL_BYTE, __LINE__, 9999, "ASCII read: %s", inASCII );
              __convertASCII2Bin(inASCII, in);
            }
          }
          else {
            int idx = 0;
            while(SerialOp.available(data->serial) > 0 && idx < 13) {
              if( SerialOp.read( data->serial, (char*)(in+idx), 1 ) ) {
                if( SerialOp.available(data->serial) == 0 )
                  ThreadOp.sleep(10);
                idx++;
              }
              else {
                break;
              }
            }
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "%d bytes read, nr of data bytes is %d", idx, in[4]&0x0F );
          }

        }
        else if( available == -1 || SerialOp.getRc(data->serial) > 0 ) {
          /* device error */
          data->conOK = False;
          SerialOp.close(data->serial);
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error" );
          ThreadOp.sleep(10);
          if( data->run ) continue;
          else break;
        }
        else {
          ThreadOp.sleep(10);
          if( data->run ) continue;
          else break;
        }
      }
      /* !data->conOK */
      else {
        ThreadOp.sleep(100);
        retry++;
        if( retry >= 50 ) {
          retry = 0;
          data->conOK = SerialOp.open( data->serial );
          if(data->conOK) {
            SerialOp.flush(data->serial);
            data->initASCII = False;
          }
        }
        if( data->run ) continue;
        else break;
      }
    }
    if( in[1] != 0xFF )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 13 );

    /* CS2 communication consists of commands (command byte always even) and replies. Reply byte is equal to command byte but with
       response bit (lsb) set, so always odd. When Rocrail sends a command, this is not broadcasted by the CS2, only the reply
       is broadcasted. When a command is issued from the CS2 user interface, both the command and the reply is broadcasted.
       This means that when a command (even) is received, Rocrail did not send that command. */
    if( in[1] == ID_SYSTEM || in[1] == (ID_SYSTEM + BIT_RESPONSE) ) {
      /*System command */
      __evaluateMCS2System( data, in );
    }
    else if( in[1] == CAN_S88_REPORT && in[4] == 7 ) {
      /* unoffcial reply to unofficial polling command, don't care if the poll was from Rocrail or not, always good to have the S88 state. */
      __evaluateMCS2S88( data, in, store );
    }
    else if( in[1] == CAN_SENSOR_EVENT ) {
      __evaluateSensorEvent( data, in );
    }
    else if( in[1] == 0x22 && in[4] == 8 ) { /* Without response bit set!? (CAN Digital Bahn) */
      __evaluateSensorEvent( data, in );
    }
    else if( in[1] ==  CAN_ID_PING ) {
      __evaluateMCS2PingCmd( data, in );
    }
     else if( in[1] == ( CAN_ID_PING | BIT_RESPONSE ) ) {
      __evaluateMCS2Ping( data, in );
    }
    else if( in[1] == ID_LOCO_DIRECTION | in[1] == 0x08 ) {
      /* loc speed or direction comamnd, not from Rocrail. */
      __evaluateMCS2Loc( data, in );
    }
    else if( in[1] == ID_LOCO_FUNCTION ) {
      /* locfunction command, not from Rocrail. */
      __evaluateMCS2Function( data, in );
    }
    else if( in[1] == ID_ACC_SWITCH && in[10] == 0x01 ) {
      /* switch command gate activated, second command with gate deactivated again is ignored, not from Rocrail. */
      __evaluateMCS2Switch( data, in );
    }
    else if( in[1] == ID_ACC_SWITCH_RSP && (in[9] & 0x80) ) {
      /* CC-Schnitte */
      __evaluateCCSwitch( data, in );
    }
    else if( in[1] == (ID_LOCO_DISCOVERY + BIT_RESPONSE) ) {
      __evaluateMCS2Discovery( data, in );
    }
    else if( in[1] == (ID_LOCO_BIND + BIT_RESPONSE) ) {
       __evaluateMCS2Bind( data, in );
    }
    else if( in[1] == (ID_LOCO_VERIFY + BIT_RESPONSE) ) {
      __evaluateMCS2Verify( data, in );
    }
    else if( in[1] == (ID_LOCO_READ_CONFIG + BIT_RESPONSE) ) {
      __evaluateMCS2ReadConfig( data, in );
    }
    else if( in[1] == (ID_LOCO_WRITE_CONFIG + BIT_RESPONSE) ) {
      __evaluateMCS2WriteConfig( data, in );
    }
    else if( in[1] == (ID_CAN_BOOT_BOUND + BIT_RESPONSE) ) {
       TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "packet : ID_CAN_BOOT_BOUND + BIT_RESPONSE");
       __evaluateMCS2CANBootBound( data, in );
    }
    else if( in[1] != 0xFF ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Unhandled packet: CAN-ID=0x%02X len=%d", in[1]&0xFF, in[4]&0x0F );
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 13 );
    }
    ThreadOp.sleep(0);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader stopped." );
}


static void __binder( iOMCS2Data data) {
  byte  buffer[32];

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Bind/Discovery option started." );
  
  iONode loco = wMCS2.getproduct(data->mcs2ini);
  while( loco != NULL ) {
    wProduct.setcid(loco, 0);
    loco = wMCS2.nextproduct(data->mcs2ini, loco);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "binder recognized devices:");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mcs2gfpUID: 0x%08X  mcs2guiUID: 0x%08X ms2UID: 0x%08X gbUID: 0x%08X", data->mcs2gfpUID, data->mcs2guiUID, data->ms2UID, data->gbUID );

  if( data->ms2UID != 0 || data->mcs2gfpUID != 0 || data->gbUID != 0 ) {
    
    loco = wMCS2.getproduct(data->mcs2ini);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Number of locos in product table: %d", __countUID(data) );
    while( data->run && loco != NULL ) {
      while( data->run && (!data->power || data->mfxDetectInProgress) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Waiting for power (%s) or mfx detection in progress (%s)", data->power?"on":"off", data->mfxDetectInProgress?"yes":"no" );
        ThreadOp.sleep(1150);
      }
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "product waiting: %s, UID: %08X, handling UID: %08X", wProduct.getdesc(loco), wProduct.getpid(loco), data->uid );
      if( data->run && data->uid == 0 && wProduct.getpid(loco) != 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Fetching product UID: %08X", wProduct.getpid(loco) );
        if( wProduct.getcid(loco) == 0 ) {
          data->uid = wProduct.getpid(loco);
          data->sid = wProduct.getsid(loco);
          StrOp.copy( idname, wProduct.getdesc(loco) );
          if( idname[0] == 0) {
            TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999, "No id in rocrail.ini for sid: %d", wProduct.getsid(loco) );
          }
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Binder: registered status for loco %s is: %d", wProduct.getdesc(loco), wProduct.getcid(loco) );
          if( (data->mcs2gfpUID == 0 || data->gbUID != 0) && wProduct.getcid(loco) == 0 ) {
            if( __findLocinList(data) && __checkForValidName(data) ) {
              wProduct.setcid(loco, 1);
            }
            byte  buffer[32];
            buffer[0]  = (wProduct.getpid(loco) & 0xFF000000) >> 24;
            buffer[1]  = (wProduct.getpid(loco) & 0x00FF0000) >> 16;
            buffer[2]  = (wProduct.getpid(loco) & 0x0000FF00) >> 8;
            buffer[3]  = (wProduct.getpid(loco) & 0x000000FF);

            if( data->ms2UID == 0 && data->mcs2guiUID == 0 ) {
              buffer[4]  = (wProduct.getsid(loco) / 256) & 0xFF;
              buffer[5]  = (wProduct.getsid(loco) % 256) & 0xFF;
              TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "bind requested for %s, pid 0x%08X, sid %d", data->id, wProduct.getpid(loco), wProduct.getsid(loco));
              ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_BIND, False, 6, buffer) );
            }
            else {
              TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "discover requested for %s, pid 0x%08X, sid %d", data->id, wProduct.getpid(loco), wProduct.getsid(loco));
              buffer[4]  = 0x20;
              buffer[5]  = 0;
              ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_DISCOVERY, True, 5, buffer) );
            }
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Product UID: %08X already registered %s SID %d", wProduct.getpid(loco), wProduct.getdesc(loco), wProduct.getsid(loco));
        }
        loco = wMCS2.nextproduct(data->mcs2ini, loco);
      }
      if( loco != NULL && wProduct.getpid(loco) == 0 ) {
          loco = wMCS2.nextproduct(data->mcs2ini, loco);
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Check your rocrail.ini it contains a corrupt row with a product UID: 0 under the controller section.");
      } 
      if( loco != NULL ) {
        ThreadOp.sleep(125);
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Binder completed." );
}


static void __GleisboxSvc( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  int i = 0;
  byte buffer[32];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Boot helper and Gleisbox Channel Reporter started on interface: %s.", data->iid );
  if( !data->power ) {
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CAN_CMD_PING, False, 0, buffer) );
  }
  ThreadOp.sleep(50); /* give time to ping to determine the CAN bus config */
/* Wait until at least one CANbus member is detected */
  if( data->ms2UID == 0 && data->mcs2guiUID == 0 && data->gbUID == 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No CS2/MS2 CAN devices detected on iid: %s, sending boot to standalone Gleisbox", data->iid );
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_CAN_BOOT_BOUND, False, 0, buffer) );
    ThreadOp.sleep(2040);
  }

  __setSwitchtime(data);

   while( data->run && i < 8 && data->ms2UID == 0 && data->mcs2gfpUID == 0){
    if( data->gbUID != 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Start configuring Gleisbox");
      buffer[4]  = CMD_SYSSUB_NEWREGNR;
      if( wMCS2.isdiscovery(data->mcs2ini) ) {
        buffer[6]  = rand() % 22;
      }
      else {
        buffer[6] = __getnewSID(data);
      }
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Set mfx sid counter to %d.", buffer[6] );
      ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 7, buffer) );
      buffer[4]  = CMD_SYSSUB_ENAPROT;
      buffer[5]  = PROT_MM2 | PROT_MFX | PROT_DCC;
      buffer[6]  = 0;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Enabling protocols: MM2, MFX & DCC");
      ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 6, buffer) );
      if( data->power) {
        buffer[4]  = CMD_SYSSUB_GO;
        buffer[5]  = 0;
        ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 5, buffer) );
      }      
      ThreadOp.sleep(2500);
      break;
    }
    if( i == 7 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "CANbus device error, no MS2 / Gleisbox detected on iid: %s", data->iid );
      break;
    }
    ThreadOp.sleep(100);
    i++;
  }


  ThreadOp.sleep(2050);
  if( wMCS2.isbind(data->mcs2ini) || wMCS2.isdiscovery(data->mcs2ini) ) {
    __binder(data);
  }
  while( data->uid != 0 || data->mfxDetectInProgress ) {
    ThreadOp.sleep(250);
  }
            
  if( wMCS2.isdiscovery(data->mcs2ini) ) {
    buffer[0] = 0;
    buffer[4] = 0;
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "mfx full discovery requested.");
    ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_LOCO_DISCOVERY, False, 1, buffer) );
  }
 
  if( wDigInt.isreportstate(data->ini) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reportGleisboxChannel started on interface: %s.", data->iid );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mgbsvc: data->mcs2gfpUID: 0x%08X data->gbUID: 0x%08X", data->mcs2gfpUID, data->gbUID );
    while( data->run && data->gbUID != 0 && wDigInt.isreportstate(data->ini) ) {
      byte buffer[32];
      buffer[0]  = (data->gbUID & 0xFF000000) >> 24;
      buffer[1]  = (data->gbUID & 0x00FF0000) >> 16;
      buffer[2]  = (data->gbUID & 0x0000FF00) >> 8;
      buffer[3]  = (data->gbUID & 0x000000FF);
      buffer[4] = CMD_SYSSUB_STATUS;
      data->lstChn++;
      switch (data->lstChn) {
        case 2:
          data->lstChn++;
          break;
        case 5:
          data->lstChn = 1;
      }
      buffer[5] = data->lstChn; /* 1= current 3= voltage 4= temp */
      ThreadOp.post( data->writer, (obj)__makeMsg(0, CMD_SYSTEM, False, 6, buffer) );
      ThreadOp.sleep(1300);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Boot helper and Gleisbox Channel Reporter ended." );
}

static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  if(wDigInt.isasciiprotocol( data->ini )) {
    while( !data->initASCII && data->run ) {
      ThreadOp.sleep(10);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer started with hash: 0x%04X", rrHash );

  do {
    byte* cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      if( data->udp ) {
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 13 );
        SocketOp.sendto( data->writeUDP, (char*)cmd, 13, NULL, 0 );
      }
      else {
        if( wDigInt.isasciiprotocol( data->ini ) ) {
          char out[64] = {'\0'};
          int len = __convertBin2ASCII(cmd, out);
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "ASCII write: %s", out );
          SerialOp.write( data->serial, out, len );
        }
        else {
          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 13 );
          SerialOp.write( data->serial, (char*)cmd, 13 );
        }
      }

      freeMem( cmd );
    }

    ThreadOp.sleep(data->cmdpause);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer stopped." );
}



/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOMCS2Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**
 * UDP Broadcast address = wDigInt.gethost()
 * UDP send port 15730 (fix)
 * UDP recv port 15731 (fix)
 *
 * */
static struct OMCS2* _inst( const iONode ini ,const iOTrace trc ) {
  iOMCS2 __MCS2 = allocMem( sizeof( struct OMCS2 ) );
  iOMCS2Data data = allocMem( sizeof( struct OMCS2Data ) );
  MemOp.basecpy( __MCS2, &MCS2Op, 0, sizeof( struct OMCS2 ), data );
  int portTX = 15731;
  int portRX = 15730;
  char procnr[2];
  char procname[17];

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  
  /* initialize random seed: */
  srand (time(NULL));

  data->ini = ini;
  data->mcs2ini = wDigInt.getmcs2(ini);
  if( data->mcs2ini == NULL ) {
    data->mcs2ini = NodeOp.inst( wMCS2.name(), NULL, ELEMENT_NODE );
    NodeOp.addChild( ini, data->mcs2ini);
  }
  data->cmdpause = wMCS2.getcmdpause(data->mcs2ini);
  data->sensorbegin = wMCS2.getsensorbegin(data->mcs2ini);
  data->sensorend   = wMCS2.getsensorend(data->mcs2ini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MGBOX %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  s88 modules      [%d]", wDigInt.getfbmod( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  version          [%d]", wDigInt.getprotver( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sensor device ID [%d]", wMCS2.getfbdevid(data->mcs2ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  command pause    [%d]ms", data->cmdpause );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sensor range     [%d-%d]", data->sensorbegin, data->sensorend );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  hash             [0x%04X]", rrHash );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  discovery        [%s]", wMCS2.isdiscovery(data->mcs2ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bind             [%s]", wMCS2.isbind(data->mcs2ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  report state     [%s]", wDigInt.isreportstate(data->ini)?"yes":"no" );

  data->udp = !StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib(data->ini));

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sublib           [%s]", wDigInt.getsublib(data->ini) );
  if( data->udp ) {
    if( wDigInt.getudpportTX(data->ini) > 0 )
      portTX = wDigInt.getudpportTX(data->ini);
    if( wDigInt.getudpportRX(data->ini) > 0 )
      portRX = wDigInt.getudpportRX(data->ini);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp address      [%s]", wDigInt.gethost(data->ini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp tx port      [%d]", portTX );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  udp rx port      [%d]", portRX );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

    data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), portRX, False, True, False );
    SocketOp.bind(data->readUDP);
    data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), portTX, False, True, False );
    data->conOK = True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device           [%s]", wDigInt.getdevice(data->ini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bps              [%d]", wDigInt.isasciiprotocol( data->ini )?115200:500000 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  SLCAN            [%s]", wDigInt.isasciiprotocol( data->ini )?"yes":"no" );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

    data->serial = SerialOp.inst( wDigInt.getdevice(data->ini) );
    SerialOp.setFlow( data->serial, cts );
    SerialOp.setLine( data->serial, wDigInt.isasciiprotocol( data->ini )?115200:500000, 8, 1, none, wDigInt.isrtsdisabled(data->ini) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );
    data->conOK = SerialOp.open( data->serial );
  }

  data->fbmod  = wDigInt.getfbmod( ini );
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->swtime = wDigInt.getswtime( ini );
  data->run    = True;
  data->ms2UID = 0;
  data->gbUID  = 0;
  data->mcs2guiUID = 0;
  data->swtimeset = False;
  data->mcs2gfpUID = 0;

  __clearRegMfxVar(data);
  
  char* readerthreadname = allocMem(16);
  char* writerthreadname = allocMem(16);
  char* fbreaderthreadname = allocMem(16);
  char* mgboxsvcthreadname = allocMem(16);

  if( instCnt==0 ) {
    StrOp.fmtb( readerthreadname, "mcs2reader");
    StrOp.fmtb( writerthreadname, "mcs2writer");
    StrOp.fmtb( fbreaderthreadname, "fbreader");
    StrOp.fmtb( mgboxsvcthreadname, "mgboxsvc");
  }
  else {
    StrOp.fmtb( readerthreadname, "%u_mcs2reader", instCnt);
    StrOp.fmtb( writerthreadname, "%u_mcs2writer", instCnt);
    StrOp.fmtb( fbreaderthreadname, "%u_fbreader", instCnt);
    StrOp.fmtb( mgboxsvcthreadname, "%u_mgboxsvc", instCnt);
  }

  data->reader = ThreadOp.inst( readerthreadname, &__reader, __MCS2 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( writerthreadname, &__writer, __MCS2 );
  ThreadOp.start( data->writer );

  data->feedbackReader = ThreadOp.inst( fbreaderthreadname, &__feedbackMCS2Reader, __MCS2 );
  ThreadOp.start( data->feedbackReader );

  data->mgboxsvc = ThreadOp.inst( mgboxsvcthreadname, &__GleisboxSvc, __MCS2 );
  ThreadOp.start( data->mgboxsvc );

  instCnt++;
  return __MCS2;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/mcs2.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
