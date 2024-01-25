using UnityEngine;

public class BallMover : MonoBehaviour
{
    public float moveSpeed = 5f; // Brzina kretanja lopte

    private Vector3 targetPosition; // Ciljna pozicija do koje se kreæe lopta
    private bool isMoving = false; // Provjera je li lopta u pokretu

    private Camera mainCamera; // Referenca na glavnu kameru

    void Start()
    {
        mainCamera = Camera.main; // Postavljamo referencu na glavnu kameru
        targetPosition = transform.position; // Poèetna ciljna pozicija je trenutna pozicija lopte
    }

    // Update se zove jednom po frejmu
    void Update()
    {
        // Provjerava se je li korisnik kliknuo lijevi gumb miša
        if (Input.GetMouseButtonDown(0))
        {
            //Pretvara poziciju klika na ekranu u zraku koja se projicira iz pozicije kamere u smjeru gdje korisnik pokazuje mišem
            Ray ray = mainCamera.ScreenPointToRay(Input.mousePosition);
            RaycastHit hit;

            // Izvršava se raycast; provjerava sijeèe li se zraka s nekim objektom u sceni
            if (Physics.Raycast(ray, out hit))
            {
                // Sadržava 3D koordinate u sceni gdje se zraka prvi put sijeèe s kolajderom
                targetPosition = hit.point;
                isMoving = true; // Postavlja se da je lopta u pokretu
            }
        }

        // Ako je lopta u pokretu i nije preblizu ciljne pozicije, pomièe se prema ciljnoj poziciji
        if (isMoving && (targetPosition - transform.position).sqrMagnitude > 0.01f) //sqrMagnitude - kvadrat udaljenosti izmeðu dvije toèkes
        {
            // Pomièe loptu prema ciljnoj poziciji
            transform.position = Vector3.MoveTowards(transform.position, new Vector3(targetPosition.x, transform.position.y, targetPosition.z), moveSpeed * Time.deltaTime);
        }
        else
        {
            isMoving = false; // Ako je lopta blizu cilja, postavlja se da više nije u pokretu
        }
    }
}
