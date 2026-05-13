<h1>Custom Reflow Hot Plate</h1>

<h3>Why did I make this?</h3>
<p>Designing and purchasing custom PCBs is expensive. For reference, my first custom devboard was quoted by JLCPCB at around $180, and more than half of that was due to assembly costs. Therefore, I sought to cut costs by designing my own at-home PCB assembly system, beginning with the "simplest" part of the process, which is being able to solder on components, both surface-mount and through-hole. This hot plate will be used to solder surface-mount components which would otherwise be extremely hard to solder by hand otherwise due to high pin density and small pin sizing.</p>

<h3>Why not just buy one?</h3>
<p>On the market the cheapest reflow hot plate is around $40 from sites which have questionable reputations. Since I have sufficient electronics components (salvaged from old projects/appliances) at home for the hot plate controller, I decided to simply build my own, only spending an extra $26 on a 240VAC relay, a cheap 600W heater element, and a thermocouple for precise temperature control.</p>

<h3>How does it work?</h3>
<p>Reflow soldering is a well-documented technique. Using a precise heating curve for four distinct stages, the process proceeds as follows: preheat, thermal soak, reflow, cooling. The thermocouple and the central controlling logic is necessary to maintain the correct temperature so that the solder paste on the board melts at the precise moment where it can then form a solid electrical connection between the pins of the component and the exposed copper on the board. 
<ol>
  <li>
    Preheat
  </li>
  <ul>
    <li>
      Heating rates need to be controlled during the preheat phase to prevent bubbling/splatter of the solder paste and warping of the components.
    </li>
  </ul>
  The thermal soak phase temperature needs to be held steady to ensure even heating. The peak temperature, achieved during reflow, needs to be tuned so that the sensitive components are not damaged while the solder paste is free to flow; due to the forces of surface tension, the paste will "pull" the pins directly over the copper on the board and ensure a strong connection. Cooling needs to be gradual so that the joints are solid and no thermal shock effects occur. </p>
</ol>
