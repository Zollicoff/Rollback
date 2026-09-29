"""Player-visible large-world contracts, observed at the cartridge boundary.

This extends coverage past one viewport: a dead zone, reverse travel, 8-bit
scroll wrap, fixed HUD bands, far-object clipping and the two world edges.
No source-coordinate reads, game RAM writes or invulnerability switches.
"""
from rom_harness import Console


def verify_scrolling(rom):
    c=Console(rom)
    try:
        c.resume(1412);c.launch();c.hold([],65)
        origin=c.save()
        c.hold(['right'],120)
        assert c.emu.screen.tilemap_position_list[64][0]>0, 'Camera must follow before the right edge'
        c.restore(origin)
        assert c.camera()==0
        assert not any(c.sprite_screen(14+2*i) for i in range(3)), 'Distant rescue pods wrapped onto the first screen'
        c.hold(['right'],12)
        assert c.camera()==0, 'Camera moved inside its resting band'
        c.hold(['right'],48)
        c.hold([],3)  # Settle the last input/OAM frame before measuring reversal.
        camera=c.camera()
        assert camera>0 and 96<=c.player()[0]-camera<=108, 'Camera did not leave room ahead of the ship'
        c.hold(['left'],20)
        assert c.camera()==camera, 'Camera jumped immediately on reversing direction'
        c.hold(['left'],72)
        assert c.camera()<camera, 'Camera did not follow back to the left'
        c.close()
        # The boss drill leaves the travel lanes clear until its eastern arena,
        # separating this camera/boundary contract from swarm-combat tactics.
        c=Console(rom);c.resume(2648);c.launch();c.hold([],65)
        c.screenshot('scrolling-west')
        c.hold(['right'],2)
        sectors=set();max_camera=0
        for step in range(400):
            c.hold(['right','a']+(['b'] if step%12==0 else []),8)
            assert c.line(0).startswith('HP'), 'Flight across the large world ended unexpectedly: '+c.text()
            camera=c.camera();max_camera=max(max_camera,camera)
            assert camera>=0 and camera<=1440
            positions=c.emu.screen.tilemap_position_list
            assert all(positions[y][0]==0 for y in (*range(16),*range(128,144))), 'The HUD scrolled with the world'
            assert all(positions[y][0]==camera%256 for y in range(16,128)), 'A terrain scanline used the wrong camera'
            sector=int(c.line(16)[15:17]);sectors.add(sector)
            if sector==5 and not hasattr(c,'mid_capture'):
                c.screenshot('scrolling-middle');c.mid_capture=True
            if c.player()[0]>=1586:break
        assert sectors==set(range(1,11)) and max_camera==1440, 'Could not reach all ten sectors'
        edge=c.player()[0]
        c.hold(['right','a'],24)
        assert abs(c.player()[0]-edge)<=1 and c.camera()==1440, 'East world boundary did not clamp'
        c.screenshot('scrolling-east')
        # A fixed menu must not reset the world camera when gameplay resumes.
        c.tap('start');assert 'TIMELINE PAUSED' in c.text()
        c.tap('start');c.hold([],40)
        assert c.line(0).startswith('HP') and c.camera()==1440, 'Pause/resume lost the far-end camera'
        c.hold(['left'],2)
        for step in range(400):
            c.hold(['left','a']+(['b'] if step%12==0 else []),8)
            assert c.line(0).startswith('HP'), 'Return flight ended unexpectedly: '+c.text()
            if c.player()[0]<=13:break
        assert c.player()[0]<=13 and c.camera()==0, 'Could not return to the west world boundary'
        c.screenshot('scrolling-return')
        return {'world_width_pixels':1600,'sectors_visited':sorted(sectors),'max_camera':max_camera,
                'checks':['camera dead zone and reversal','all ten sectors and scroll-register wraps',
                          'fixed HUD on every sampled scanline','far-object clipping',
                          'both world boundaries','far-camera pause/resume','return travel']}
    finally:
        c.close()


if __name__=='__main__':
    import json,sys
    print(json.dumps(verify_scrolling(sys.argv[1]),indent=2))
