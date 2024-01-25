using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CreateGrass : MonoBehaviour
{
    public GameObject grassPrefab; // Prefab za travu koji æe se instancirati.
    public int grassSize = 10; // Velièina podruèja trave koje æe se generirati.
    public GameObject ball; // Referenca na objekt lopte u sceni.
    public Material mat; // Materijal trave koji æe se ažurirati.

    // Metoda Start se poziva prije prvog frame-a kad se scena uèita.
    void Start()
    {
        // Dvostruka for petlja koja generira travu u kvadratnom podruèju.
        for (int z = -grassSize; z <= grassSize; z++)
        {
            for (int x = -grassSize; x <= grassSize; x++)
            {
                // Izraèunavamo poziciju za svaki komad trave, s malom randomizacijom
                // da izgleda prirodnije.
                Vector3 position = new Vector3(x / 4.0f + Random.Range(-0.25f, 0.25f), 0, z / 4.0f + Random.Range(-0.25f, 0.25f));
                // Instanciramo travu na izraèunatoj poziciji s default rotacijom (Quaternion.identity).
                GameObject grass = Instantiate(grassPrefab, position, Quaternion.identity);
                // Randomiziramo visinu trave za varijabilniji izgled.
                grass.transform.localScale = new Vector3(1, Random.Range(0.2f, 0.8f), 1);
            }
        }
    }

    // Metoda Update se poziva svaki frame.
    void Update()
    {
        // Postavljamo vektor _TramplePosition u materijalu trave na poziciju lopte.
        // Ovo se može koristiti za efekt gdje trava reagira kada lopta prelazi preko nje,
        // na primjer, da se spljošti ili promijeni boju na mjestu gdje lopta dotakne travu.
        mat.SetVector("_TramplePosition", ball.transform.position);
    }
}
