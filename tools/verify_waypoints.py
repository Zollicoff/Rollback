"""Observe navigation arrows in actual ROM OAM/framebuffers, using buttons only."""
from rom_harness import Console

def verify_waypoints(rom):
    for index,palette,name in [(0,3,'protect'),(1,1,'attack'),(2,6,'rescue'),
                               (5,2,'sabotage'),(8,6,'navigate'),(11,1,'boss')]:
        c=Console(rom)
        try:
            c.resume(2648)
            for _ in range(11-index): c.tap('left',after=3)
            c.launch(); c.hold([],65)
            arrow=c.sprite_screen(38)
            assert arrow and (arrow[3]&7)==palette,(name,arrow)
            assert 74<=arrow[2]<90 and arrow[2]%2==0,(name,arrow)
            assert 12<=arrow[0]<=156 and 12<=arrow[1]<=104,(name,arrow)
            c.screenshot('waypoint-'+name)
            # Swarm missions acquire a living enemy independently of objectives.
            if index==0:
                for _ in range(300):
                    c.tick()
                    threat=c.sprite_screen(39)
                    if threat: break
                assert threat and (threat[3]&7)==1,'Missing red threat arrow'
                assert 74<=threat[2]<90
                c.screenshot('waypoint-protect-and-enemy')
        finally:
            c.close()
    return ['objective/threat arrow visibility, target colors and playfield bounds']
