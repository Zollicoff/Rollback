"""Two-axis camera/world contracts observed through buttons and LCD output.

No source-coordinate reads, game RAM writes or invulnerability switches.
The previous horizontal-only build fails the first vertical follow assertion.
"""
import numpy as np
from rom_harness import Console


def verify_scrolling(rom):
    c=Console(rom)
    try:
        c.resume(1412);c.launch();c.hold([],65)
        origin=c.save()
        c.hold(['down'],120)
        assert c.emu.screen.tilemap_position_list[64][1]>0, 'Camera must follow vertically before the bottom edge'
        c.restore(origin)
        assert c.camera()==0 and c.camera_y()==0
        assert not any(c.sprite_screen(14+2*i) for i in range(3)), 'Distant rescue pods wrapped onto the first screen'
        for forward,reverse,axis,short in [('right','left',0,12),('down','up',1,4)]:
            c.restore(origin)
            camera=c.camera if axis==0 else c.camera_y
            if axis==1:c.hold([reverse],16)
            c.hold([forward],short)
            assert camera()==0, 'Camera moved inside its resting band'
            c.hold([forward],48);c.hold([],8)
            offset=camera()
            expected=104 if axis==0 else 72
            assert offset>0 and abs(c.player()[axis]-offset-expected)<=2, 'Camera did not leave padding before the edge'
            c.hold([reverse],12)
            assert camera()==offset, 'Camera jumped immediately on reversal'
            c.hold([reverse],90)
            assert camera()<offset, 'Camera did not follow back'
        c.close()
        # The boss drill separates travel/boundary checks from swarm tactics.
        c=Console(rom);c.resume(2648);c.launch();c.hold([],65)
        c.screenshot('world-northwest')
        start_map=[[c.tile(x,y) for x in range(20)] for y in range(14)]
        frame=np.array(c.emu.screen.image)
        hp_pixels=frame[112:120,:16].copy();target_pixels=frame[128:136,:48].copy()
        columns=set();rows=set();max_x=max_y=0
        legs=[('right',0,1587,'world-northeast'),('down',1,1411,'world-southeast'),
              ('left',0,12,'world-southwest'),('up',1,28,'world-return')]
        for direction,axis,edge,name in legs:
            c.hold([direction],2)
            for step in range(600):
                c.hold([direction,'a']+(['b'] if 'B OK' in c.line(0) else []),8)
                if not c.line(0).startswith('HP'):
                    c.screenshot('world-first-failure-'+direction)
                    c.hold([],40)
                    raise AssertionError(f'World traversal ended during {direction} at step {step}: '+c.text())
                cx,cy=c.camera(),c.camera_y();max_x=max(max_x,cx);max_y=max(max_y,cy)
                assert 0<=cx<=1440 and 0<=cy<=1328
                positions=c.emu.screen.tilemap_position_list
                assert all(positions[y][:2]==[cx%256,cy%256] for y in range(112)), 'Terrain scanlines disagree with the camera'
                frame=np.array(c.emu.screen.image)
                assert np.array_equal(frame[112:120,:16],hp_pixels), 'Health HUD moved with the world'
                assert np.array_equal(frame[128:136,:48],target_pixels), 'Navigation HUD moved with the world'
                columns.add(int(c.line(16)[15:17]));rows.add(int(c.line(16)[18:20]))
                if abs(c.player()[axis]-edge)<=1:break
            assert abs(c.player()[axis]-edge)<=1, 'Could not reach '+name
            c.hold([direction,'a'],24)
            assert abs(c.player()[axis]-edge)<=1, 'World boundary did not clamp at '+name
            c.screenshot(name)
            if name=='world-southeast':
                c.tap('start');assert 'TIMELINE PAUSED' in c.text()
                c.tap('start');c.hold([],40)
                assert (c.camera(),c.camera_y())==(1440,1328), 'Pause/resume lost the distant camera'
        assert columns==set(range(1,11)) and rows==set(range(1,11)), 'Not all world columns/rows were reached'
        assert (max_x,max_y)==(1440,1328) and (c.camera(),c.camera_y())==(0,0)
        returned=[[c.tile(x,y) for x in range(20)] for y in range(14)]
        assert returned==start_map, 'Streamed terrain changed on returning to the same viewport'
        # Move to a clear lane before a diagonal pan; both offsets must change.
        c.hold(['right'],70);c.hold(['down'],60)
        for _ in range(35):c.hold(['right','down'],8)
        assert c.camera()>40 and c.camera_y()>40, 'Diagonal flight did not pan both axes'
        c.screenshot('world-diagonal')
        return {'world_width_pixels':1600,'world_height_pixels':1440,
                'columns_visited':sorted(columns),'rows_visited':sorted(rows),
                'max_camera':[max_x,max_y],
                'checks':['two-axis dead zones and reversals','ten world columns and ten world rows',
                          'horizontal and vertical scroll-register wraps','fixed HUD pixels',
                          'distant-object clipping','four world boundaries and corners',
                          'far-camera pause/resume','terrain restoration after return','diagonal camera motion']}
    finally:
        c.close()


if __name__=='__main__':
    import json,sys
    print(json.dumps(verify_scrolling(sys.argv[1]),indent=2))
