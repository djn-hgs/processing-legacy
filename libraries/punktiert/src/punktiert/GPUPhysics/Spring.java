package punktiert.GPUPhysics;

import edu.syr.pcpratts.rootbeer.runtime.Kernel;
import punktiert.math.Vec;


/**
 * <p>
 * A spring class connecting two GPUParticles in space.
 * OpenCL integration with the help of rotbeer (c) 2012 Phil Pratt-Szeliga, Syracuse University
 * Rootbeer is licensed under the MIT license,  https://github.com/pcpratts/rootbeer1
 */

public class Spring implements Kernel{

	protected static final float EPS = 1e-6f;
	/**
	 * Spring end points A,B
	 */
	public Particle a, b;
	/**
	 * Spring rest length to which it always wants to return too
	 */
	protected float restLength;
	protected double restLengthSquared;
	/**
	 * Spring strength, possible value range depends on engine configuration
	 */
	protected float strength;

	/**
	 * Spring constructor: takes as input Endpoints A,B, desired Restlength of the spring, strength of the connection
	 * 
	 * @param a 1st particle
	 * @param b 2nd particle
	 * @param len desired rest length
	 * @param str spring strength
	 */
	public Spring(Particle a, Particle b, float len, float str) {
		this.a = a;
		this.b = b;
		restLength = len;
		restLengthSquared = restLength*restLength;
		strength = str;
	}
	/**
	 * Spring constructor: takes as input Endpoints A,B; strength of the connection; Restlength based on the distance between the endpoints
	 * 
	 * @param a 1st particle
	 * @param b 2nd particle
	 * @param str spring strength
	 */
	public Spring(Particle a, Particle b, float str) {
		this.a = a;
		this.b = b;
		restLength = a.dist(b);
		restLengthSquared = restLength*restLength;
		strength = str;
	}
/*
	public final float getRestLength() {
		return restLength;
	}

	public final float getStrength() {
		return strength;
	}
*/
	public Spring setRestLength(float len) {
		restLength = len;
		restLengthSquared = len * len;
		return this;
	}

	public Spring setStrength(float strength) {
		this.strength = strength;
		return this;
	}
	/**
	 * Updates both particle positions (if not locked) based on their current distance, weight and spring configuration *
	 */
	 @Override
	public void gpuMethod() {
		Vec delta = b.sub(a);
		// add minute offset to avoid div-by-zero errors
		float dist = delta.mag() + EPS;
		float normDistStrength = (dist - restLength) / (dist * (a.invWeight + b.invWeight)) * strength;
		if (!a.isLocked) {
			a.addSelf(delta.mult(normDistStrength * a.invWeight));	
		}
		if (!b.isLocked) {
			b.addSelf(delta.mult(-normDistStrength * b.invWeight));
		}
	}

}
